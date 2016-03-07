#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <unistd.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/uuid.h>

#include "att.h"
#include "btio.h"
#include "gattrib.h"
#include "gatt.h"
#include "gatttool.h"


#define FLAGS_LIMITED_MODE_BIT 0x01
#define FLAGS_GENERAL_MODE_BIT 0x02
#define EIR_NAME_SHORT              0x08  /* shortened local name */
#define EIR_NAME_COMPLETE           0x09  /* complete local name */

/*				
//''Higher level'' (Are caused by lower level error(s)):
#define ECONNECTFAILED			1					
#define EDISCONNECTFAILED		2					
#define EAPPENDCHECKSUMFAILED	3					
#define EWRITEFAILED			4					
#define EREADFAILED				5		

//''Lower level'':
#define ENODEV				1					
#define ENOSERVER			2					
#define ENACKRECEIVED		3					
#define ECHECKSUMMISMATCH	4					
#define EBUFFEROVERFLOW		5		
*/
/*
	const unsigned char *primaryErrorStrMsg[6] = {	"No errors",
													"Connection Failed",
													"Disconection Failed",
													"AppendChecksum Failed",
													"Write Failed",
													"Read Failed"};
												
	const unsigned char *secondaryErrorStrMsg[6] = {	"No errors",
														"No local bluetooth device as been found",
														"The specified server is not reachable",
														"Received a NACK from the server",
														"The localy computed checksum is different than the one in the data received",
														"Buffer Overflow"};
														
	typedef struct errorStatusStruct{
		int primaryErrorCode = 0;
		int secondaryErrorCode = 0;
	}errorStatus;
	*/
	
	

	typedef void (*connect_cb)(gpointer user_data, int errorCode);
	typedef void (*write_cb)(int errorCode);
	typedef void (*listen_cb)(int errorCode);
	typedef void (*read_cb)(uint8_t *value,int vlen, int errorCode);
	
	static connect_cb userConCb;
	static write_cb userWriteCb;
	static listen_cb userListenCb;
	static read_cb userReadCb;
	
	static FILE *userFilePtr = NULL;
	
	
// "Private Functions"

//int getChecksum(char *data);

void connectCallbackWrapper(GIOChannel *io, GError *err, gpointer user_data)
{
	printf("API Connection Callback Wrapper\n");
	
	GAttrib *attrib;

	if (err) {
		g_printerr("%s\n", err->message);
			//got_error = TRUE;
			//g_main_loop_quit(event_loop);
	}

	attrib = g_attrib_new(io);
	userConCb(attrib,0);
	
}

void writeCallBackWrapper(guint8 status, const guint8 *pdu, guint16 plen, gpointer user_data)
{
	g_print("API Write Callback wrapper\n");
	
	if (status != 0) {
		g_printerr("Characteristic Write Request failed: "
						"%s\n", att_ecode2str(status));
	}
	else if (!dec_write_resp(pdu, plen)) {
		g_printerr("Protocol error\n");
	}

	userWriteCb(0);
}

void readCallBackWrapper(guint8 status, const guint8 *pdu, guint16 plen, gpointer user_data)
{
		g_print("API Read Callback wrapper\n");
		
	uint8_t value[ATT_MAX_MTU];
	int i, vlen;

	if (status != 0) {
		g_printerr("Characteristic value/descriptor read failed: %s\n",
							att_ecode2str(status));
		//goto done;
	}
	if (!dec_read_resp(pdu, plen, value, &vlen)) {
		g_printerr("Protocol error\n");
		//goto done;
	}
	//g_print("Characteristic value/descriptor: ");
	//for (i = 0; i < vlen; i++)
		//g_print("%02x ", value[i]);
	//g_print("\n");
	userReadCb(value,vlen,0);

}


// "Public Functions"
//Return 0 if ok, else return primaryErrorCode
int connect_by_addr(char *server_mac_addr, connect_cb cbFct)
{
	printf("API Connect By Address\n");
	GIOChannel *chan;
	gchar *opt_src = NULL;
	gchar *opt_dst = NULL;
	gchar *opt_sec_level = NULL;
	int opt_mtu = 0;
	int opt_psm = 0;		
			
	opt_dst = (char *)malloc(18);
	strcpy(opt_dst,server_mac_addr);
	
	opt_sec_level = (char *)malloc(4);
	strcpy(opt_sec_level,"low");
	
	userConCb = cbFct;
	
	chan = gatt_connect(opt_src, opt_dst, opt_sec_level, opt_psm, opt_mtu, connectCallbackWrapper);
	
	free(opt_src);
	free(opt_dst);
	free(opt_sec_level);
	
	return 0;
}

static int check_report_filter(uint8_t procedure, le_advertising_info *info)
{
	uint8_t flags;

	/* If no discovery procedure is set, all reports are treat as valid */
	if (procedure == 0)
		return 1;

	/* Read flags AD type value from the advertising report if it exists */
	if (read_flags(&flags, info->data, info->length))
		return 0;

	switch (procedure) {
	case 'l': /* Limited Discovery Procedure */
		if (flags & FLAGS_LIMITED_MODE_BIT)
			return 1;
		break;
	case 'g': /* General Discovery Procedure */
		if (flags & (FLAGS_LIMITED_MODE_BIT | FLAGS_GENERAL_MODE_BIT))
			return 1;
		break;
	default:
		fprintf(stderr, "Unknown discovery procedure\n");
	}

	return 0;
}

static void eir_parse_name(uint8_t *eir, size_t eir_len,
						char *buf, size_t buf_len)
{
	size_t offset;

	offset = 0;
	while (offset < eir_len) {
		uint8_t field_len = eir[0];
		size_t name_len;

		/* Check for the end of EIR */
		if (field_len == 0)
			break;

		if (offset + field_len > eir_len)
			goto failed;

		switch (eir[1]) {
		case EIR_NAME_SHORT:
		case EIR_NAME_COMPLETE:
			name_len = field_len - 1;
			if (name_len > buf_len)
				goto failed;

			memcpy(buf, &eir[2], name_len);
			return;
		}

		offset += field_len + 1;
		eir += field_len + 1;
	}

failed:
	snprintf(buf, buf_len, "(unknown)");
}

static int print_advertising_devices(int dd, uint8_t filter_type, gchar *opt_dst_name, gchar *opt_dst)
{
	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	struct hci_filter nf, of;
	struct sigaction sa;
	socklen_t olen;
	int len;

	olen = sizeof(of);
	if (getsockopt(dd, SOL_HCI, HCI_FILTER, &of, &olen) < 0) {
		printf("Could not get socket options\n");
		return -1;
	}

	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);

	if (setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0) {
		printf("Could not set socket options\n");
		return -1;
	}

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = NULL;
	sigaction(SIGINT, &sa, NULL);

	while (1) {
		evt_le_meta_event *meta;
		le_advertising_info *info;
		char addr[18];

		while ((len = read(dd, buf, sizeof(buf))) < 0) {

			if (errno == EAGAIN || errno == EINTR)
				continue;
			goto done;
		}

		ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
		len -= (1 + HCI_EVENT_HDR_SIZE);

		meta = (void *) ptr;

		if (meta->subevent != 0x02)
			goto done;

		/* Ignoring multiple reports */
		info = (le_advertising_info *) (meta->data + 1);
		if (check_report_filter(filter_type, info)) {
			char name[30];

			memset(name, 0, sizeof(name));

			ba2str(&info->bdaddr, addr);
			eir_parse_name(info->data, info->length,
							name, sizeof(name) - 1);

			printf("%s %s\n", addr, name);
			if(strcmp(name,opt_dst_name)==0)
			{
				printf("Found %s! MAC Addr: %s\n", name, addr);

				strcpy(opt_dst,addr);
				goto done;
			}
		}
	}

done:
	setsockopt(dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));

	if (len < 0)
		return -1;

	return 0;
}

//Return 0 if ok, else return primaryErrorCode
int connect_by_name(char *server_name, connect_cb cbFct)
{
	gchar *opt_dst = NULL;
	gchar *opt_dst_name = NULL;
	int dev_id =-1;
	int err, opt, dd;
	uint8_t own_type = 0x00;
	uint8_t scan_type = 0x01;
	uint8_t filter_type = 0;
	uint16_t interval = htobs(0x0010);
	uint16_t window = htobs(0x0010);
	uint8_t filter_dup = 1;
	
	opt_dst = (char *)malloc(18);

	opt_dst_name = (char *)malloc(strlen(server_name)+1);
	strcpy(opt_dst_name,server_name);
	
	// Identifying by name... to get MAC adress (see tools/hcitool.c)
		printf("Searching for device named: %s\n",opt_dst_name);
	
		dev_id = hci_get_route(NULL);

		dd = hci_open_dev(dev_id);
		err = hci_le_set_scan_parameters(dd, scan_type, interval, window, own_type, 0x00, 1000);
		err = hci_le_set_scan_enable(dd, 0x01, filter_dup, 1000);
		err = print_advertising_devices(dd, filter_type,opt_dst_name,opt_dst);
		err = hci_le_set_scan_enable(dd, 0x00, filter_dup, 1000);
		hci_close_dev(dd);

		
		connect_by_addr(opt_dst, cbFct);
		
		free(opt_dst_name);
		free(opt_dst);
}

//Write automatically append the checksum (TODO: If needed?)
//Return 0 if ok, else return primaryErrorCode

int writeSingle(gpointer user_data, int hnd,char *data, write_cb cbFct)
{
	GAttrib *attrib = user_data;
		
	printf("API Write Single\n");
		
	//Set user callback function
	userWriteCb = cbFct;
	
	//Parse attribute data and get lengeh
	uint8_t *value;
	size_t len;
	len = gatt_attr_data_from_string(data, &value);
	
	//Write --> ... --> writeCallBackWrapper
	gatt_write_char(attrib, hnd, value, len, writeCallBackWrapper, NULL);
}

//int writeBatch(GIOChannel *io, int *hnd,char **data, cb_fct cbFct); //Return 0 if ok, else return primaryErrorCode

int readSingle(gpointer user_data, int hnd,read_cb cbFct)
{
		GAttrib *attrib = user_data;
			printf("API Read Single\n");
			
			userReadCb = cbFct;
			
		gatt_read_char(attrib, hnd, 0, readCallBackWrapper, attrib);
}
//char* readBatch(GIOChannel *io, int *hnd,char **data,read_cb cbFct);

//void printErrors(FILE *fp); // Allow to redirect to sdtout or a file(eg: errorsLog)

static void events_handler(const uint8_t *pdu, uint16_t len, gpointer user_data)
{
	GAttrib *attrib = user_data;
	FILE *fp;
	if(userFilePtr != NULL)
	{
		fp = userFilePtr;
	}
	else
	{
		fp = stdout;
	}
		
	printf("API Event Handler\n");
	uint8_t opdu[ATT_MAX_MTU];
	uint16_t handle, i, olen = 0;

	handle = att_get_u16(&pdu[1]);

	switch (pdu[0]) {
	case ATT_OP_HANDLE_NOTIFY:
		g_fprintf(fp,"Notification handle = 0x%04x value: ", handle);
		break;
	case ATT_OP_HANDLE_IND:
		g_fprintf(fp,"Indication   handle = 0x%04x value: ", handle);
		break;
	default:
		g_fprintf(fp,"Invalid opcode\n");
		return;
	}

	for (i = 3; i < len; i++)
		g_fprintf(fp,"%02x ", pdu[i]);

	g_fprintf(fp,"\n");

	if (pdu[0] == ATT_OP_HANDLE_NOTIFY)
		return;

	olen = enc_confirmation(opdu, sizeof(opdu));

	if (olen > 0)
		g_attrib_send(attrib, 0, opdu[0], opdu, olen, NULL, NULL, NULL);
}

static gboolean listen_stop(gpointer user_data)
{	
	printf("API Listen stop\n");
	GAttrib *attrib = user_data;
	g_attrib_unregister_all(attrib);
	
	return FALSE;
}

static void listen_destroy(gpointer user_data)
{
	printf("API Listen destroy\n");
	userListenCb(0);
}

static void listen_start(gpointer user_data, unsigned int timeout, listen_cb cbFct, FILE *fp)
{
	GAttrib *attrib = user_data;
	
	userListenCb = cbFct;
	
	if(fp != NULL)
	{
		userFilePtr = fp;
	}
	else
	{
		userFilePtr = NULL;
	}
		
	printf("API Listen start\n");
	g_attrib_register(attrib, ATT_OP_HANDLE_NOTIFY, events_handler,
							attrib, NULL);
	g_attrib_register(attrib, ATT_OP_HANDLE_IND, events_handler,
							attrib, NULL);
							
	if(timeout)
	{
		g_timeout_add_full (G_PRIORITY_DEFAULT,timeout, listen_stop, attrib, listen_destroy);
	}
}




// SIMPLE EXAMPLE USER CODE
static GMainLoop *my_event_loop;
static GAttrib *my_attrib;

static int testCnt = 0;

	void my_read_cb(uint8_t *value,int vlen, int errorCode)
	{
			int i;
				printf("User Read Callback\n");
			g_print("Characteristic value/descriptor: ");
				for (i = 0; i < vlen; i++)
				{
					g_print("%02x ", value[i]);
				}
				g_print("\n");
				
						printf("Sleeping for 10 seconds...\n");
		sleep(10);
		printf("See you soon!\n");
		g_main_loop_quit(my_event_loop);
	}
	
	void my_listen_cb(int errorCode)
	{
		printf("User Listen Callback\n");
		readSingle(my_attrib,0x0010,my_read_cb);
		//printf("Sleeping for 10 seconds...\n");
		//sleep(10);
		//printf("See you soon!\n");
		//g_main_loop_quit(my_event_loop);
	}

	void my_write_cb(int errorCode)
	{
		printf("User Write Callback\n");
		if(testCnt == 0)
		{
			writeSingle(my_attrib,0x000E,"A13C22",my_write_cb);
			testCnt = 1;
		}
		else
		{
			listen_start(my_attrib, 5000,my_listen_cb,stdout); // Can pass stdout or any other file ptr, if NULL API will use stdout, 0 as timeout=runs forever
		}
	}
	void my_con_cb(gpointer user_data, int errorCode)
	{
		printf("User Connection Callback\n");
		my_attrib = user_data;
		writeSingle(my_attrib,0x0011,"0300",my_write_cb);
	}

int main(int argc, char *argv[])
{
			printf("User Main\n");
			if(strcmp(argv[1],"-b") == 0)
			{
				connect_by_addr(argv[2], my_con_cb);		
			}
			else if(strcmp(argv[1],"-n") == 0)
			{
				connect_by_name(argv[2], my_con_cb);
			}
				my_event_loop = g_main_loop_new(NULL, FALSE);
			g_main_loop_run(my_event_loop);
			g_main_loop_unref(my_event_loop);
	exit(EXIT_SUCCESS);
}
