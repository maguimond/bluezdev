#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <glib.h>
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
	
	
	
	
	//typedef void (*cb_fct)(GIOChannel *io, int errorCode);
	//typedef void (*cb_fct)(GIOChannel *io, int status, int errorCode);
	//typedef void (*read_cb)(GIOChannel *io,uint8_t *value, int errorCode);
	//typedef void(*listen_cb)(GIOChannel *io,uint8_t *value, int errorCode);
	typedef void (*connect_cb)(GAttrib *attrib, int errorCode);
	typedef void (*write_cb)(int errorCode);
	//typedef void (*cb_fct)(GAttrib *attrib, int status, int errorCode);
	
	static connect_cb userConCb;
	static write_cb userWriteCb;

	
// "Private Functions" TODO: Call the passed callback function at the end of these callbackwrapper
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

//void readCallBackWrapper(guint8 status, const guint8 *pdu, guint16 plen, gpointer user_data);


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
//Return 0 if ok, else return primaryErrorCode
//int connect_by_name(char *server_name, connect_cb cbFct)
//{
//	
//}
//Return 0 if ok, else return primaryErrorCode
//int disconnect_by_id(int local_dev_id)
//{
//	
//}
//int disconnect_by_channel(GIOChannel *io);//Return 0 if ok, else return primaryErrorCode
//Write automatically append the checksum (TODO: If needed?)
//Return 0 if ok, else return primaryErrorCode

int writeSingle(GAttrib *attrib, int hnd,char *data, write_cb cbFct)
{
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
//char* readSingle(GIOChannel *io, int hnd,char *data,read_cb cbFct); //Return pointer to data array, should be same if it was properly alloc before called else realloc were performed, returns null on error
//char* readBatch(GIOChannel *io, int *hnd,char **data,read_cb cbFct); //Return pointer to data array, should be same if it was properly alloc before called else realloc were perfomed, returns null on error
//void printErrors(FILE *fp); // Allow to redirect to sdtout or a file(eg: errorsLog)
//void printListenEvents(GIOChannel *io, FILE *fp, int timeout, listen_cb cbFct) // Allow to redirect to stdout or a file (eg: serviceBuffer) - 0 timeout is infinite
static void events_handler(const uint8_t *pdu, uint16_t len, GAttrib *attrib)
{
	printf("API Event Handler\n");
	uint8_t opdu[ATT_MAX_MTU];
	uint16_t handle, i, olen = 0;

	handle = att_get_u16(&pdu[1]);

	switch (pdu[0]) {
	case ATT_OP_HANDLE_NOTIFY:
		g_print("Notification handle = 0x%04x value: ", handle);
		break;
	case ATT_OP_HANDLE_IND:
		g_print("Indication   handle = 0x%04x value: ", handle);
		break;
	default:
		g_print("Invalid opcode\n");
		return;
	}

	for (i = 3; i < len; i++)
		g_print("%02x ", pdu[i]);

	g_print("\n");

	if (pdu[0] == ATT_OP_HANDLE_NOTIFY)
		return;

	olen = enc_confirmation(opdu, sizeof(opdu));

	if (olen > 0)
		g_attrib_send(attrib, 0, opdu[0], opdu, olen, NULL, NULL, NULL);
}

static void listen_start(GAttrib *attrib)
{
	printf("API Listen start\n");
	g_attrib_register(attrib, ATT_OP_HANDLE_NOTIFY, events_handler,
							attrib, NULL);
	g_attrib_register(attrib, ATT_OP_HANDLE_IND, events_handler,
							attrib, NULL);

}


// SIMPLE EXAMPLE USER CODE
static GMainLoop *my_event_loop;
static GAttrib *my_attrib;

static int testCnt = 0;

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
			listen_start(my_attrib);
			//sleep(5);
			//g_main_loop_quit(my_event_loop);
		}
	}
	void my_con_cb(GAttrib *attrib, int errorCode)
	{
		printf("User Connection Callback\n");
		my_attrib = attrib;
		writeSingle(attrib,0x0011,"0300",my_write_cb);
	}

int main(int argc, char *argv[])
{
			printf("User Main\n");
			connect_by_addr(argv[2], my_con_cb);
				my_event_loop = g_main_loop_new(NULL, FALSE);
			g_main_loop_run(my_event_loop);
			g_main_loop_unref(my_event_loop);
	exit(EXIT_SUCCESS);
}
