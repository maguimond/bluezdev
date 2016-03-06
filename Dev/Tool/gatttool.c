/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2010  Nokia Corporation
 *  Copyright (C) 2010  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

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


/* Unofficial value, might still change */
#define LE_LINK		0x03

#define FLAGS_AD_TYPE 0x01
#define FLAGS_LIMITED_MODE_BIT 0x01
#define FLAGS_GENERAL_MODE_BIT 0x02

#define EIR_FLAGS                   0x01  /* flags */
#define EIR_UUID16_SOME             0x02  /* 16-bit UUID, more available */
#define EIR_UUID16_ALL              0x03  /* 16-bit UUID, all listed */
#define EIR_UUID32_SOME             0x04  /* 32-bit UUID, more available */
#define EIR_UUID32_ALL              0x05  /* 32-bit UUID, all listed */
#define EIR_UUID128_SOME            0x06  /* 128-bit UUID, more available */
#define EIR_UUID128_ALL             0x07  /* 128-bit UUID, all listed */
#define EIR_NAME_SHORT              0x08  /* shortened local name */
#define EIR_NAME_COMPLETE           0x09  /* complete local name */
#define EIR_TX_POWER                0x0A  /* transmit power level */
#define EIR_DEVICE_ID               0x10  /* device ID */




static char *opt_src = NULL;
static char *opt_dst = NULL;
static char *opt_dst_name = NULL;
static char *opt_value = NULL;
static char *opt_sec_level = NULL;
static bt_uuid_t *opt_uuid = NULL;
static int opt_start = 0x0001;
static int opt_end = 0xffff;
static int opt_handle = -1;
static int opt_mtu = 0;
static int opt_psm = 0;
static int opt_offset = 0;
static int opt_primary = 0;
static int opt_characteristics = 0;
static int opt_char_read = 0;
static int opt_listen = 0;
static int opt_char_desc = 0;
static int opt_char_write = 0;
static int opt_char_write_req = 0;
static int opt_interactive = 0;
static int got_error = 0;

static GMainLoop *event_loop;

static GAttrib *attrib;

static int test =0;

static volatile int signal_received = 0;

static void characteristics_write_req(gpointer user_data);

static void events_handler(const uint8_t *pdu, uint16_t len, gpointer user_data)
{
	GAttrib *attrib = user_data;
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

static void listen_start(gpointer user_data)
{
	GAttrib *attrib = user_data;

	g_attrib_register(attrib, ATT_OP_HANDLE_NOTIFY, events_handler,
							attrib, NULL);
	g_attrib_register(attrib, ATT_OP_HANDLE_IND, events_handler,
							attrib, NULL);

}




static void char_write_req_cb(guint8 status, const guint8 *pdu, guint16 plen, gpointer user_data)
{
	if (status != 0) {
		g_printerr("Characteristic Write Request failed: "
						"%s\n", att_ecode2str(status));
	}
	else if (!dec_write_resp(pdu, plen)) {
		g_printerr("Protocol error\n");
	}
	else {
		g_print("Characteristic value was written successfully\n");
		
if(!test)
{
		opt_value = (char *)malloc(7*sizeof(char));
			strcpy(opt_value, "A13C22");
	
			opt_handle = 0x000E;
			characteristics_write_req(attrib);
			test = 1;
}
else
{
listen_start(attrib);
}
	}
	

}

static void characteristics_write_req(gpointer user_data)
{
	//printf("char write req\n");
	//GAttrib *attrib = user_data;
	attrib = user_data;
	uint8_t *value;
	size_t len;
	
	len = gatt_attr_data_from_string(opt_value, &value);
	
	//printf("opt_value: %s\n",opt_value);
	//printf("opt_hnd: %d\n",opt_handle);
	gatt_write_char(attrib, opt_handle, value, len, char_write_req_cb, NULL);
}

static void connect_cb(GIOChannel *io, GError *err, gpointer user_data)
{
	int quitSignal =0;
		
		//GAttrib *attrib;



	if (err) {
		g_printerr("%s\n", err->message);
		got_error = 1;
		g_main_loop_quit(event_loop);
	}




	//while(!quitSignal)
	//{

 // GString *msg = NULL;
 // gchar *p;
 // gchar buf[16];

		gchar c;


		system("clear");

		printf("GeniAle Bluetooth Client v0.1 - Based on Bluez4.99\n\n");
		printf("Sucessfully connected to: %s\n\n\n",opt_dst);
		printf("Commands:\n");
		printf("1) Get Sensor Data Continuously\n");
		printf("2) Get Sensor Data Once\n");
		printf("3) Get Sensor Config\n");
		printf("4) Set Sensor Config\n\n\n");

  		//msg = g_string_sized_new(16);
  		//printf ("Enter an option: ");
  		//p = fgets(buf, sizeof(buf), stdin);
  		//g_string_assign(msg, buf);
  		//printf("msg->str = [%s] \n", (char *) msg->str);
  		//printf("msg->len = %u \n", (size_t) msg->len);
  		//printf("msg->allocated_len = %u \n", (size_t) msg->allocated_len);
		//switch (msg->str)

		printf("Enter an option: ");
		c = getchar();
		switch(c)
		{
			case '1':	attrib = g_attrib_new(io);
				opt_value = (char *)malloc(5*sizeof(char));
				strcpy(opt_value, "0300");
				opt_handle = 0x0011;
				characteristics_write_req(attrib);
				break;
			case '2':
				break;
			case '3':
				break;
			case '4':
				break;
			default:
				printf("Bad command\n");
						g_main_loop_quit(event_loop);
				break;
		}
	//}




		

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

static void sigint_handler(int sig)
{
	signal_received = sig;
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

static int print_advertising_devices(int dd, uint8_t filter_type)
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
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);

	while (1) {
		evt_le_meta_event *meta;
		le_advertising_info *info;
		char addr[18];

		while ((len = read(dd, buf, sizeof(buf))) < 0) {
			if (errno == EINTR && signal_received == SIGINT) {
				len = 0;
				goto done;
			}

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
				opt_dst = (char *)malloc(18);
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

int main(int argc, char *argv[])
{

	GIOChannel *chan;





if(strcmp(argv[1],"-n") == 0)
{
	
int dev_id =-1;
	int err, opt, dd;
	uint8_t own_type = 0x00;
	uint8_t scan_type = 0x01;
	uint8_t filter_type = 0;
	uint16_t interval = htobs(0x0010);
	uint16_t window = htobs(0x0010);
	uint8_t filter_dup = 1;

	opt_dst_name = (char *)malloc(strlen(argv[2])+1);
	strcpy(opt_dst_name,argv[2]);
	
	// Identifying by name... to get MAC adress (see tools/hcitool.c)
		printf("Searching for device named: %s\n",opt_dst_name);
	
		dev_id = hci_get_route(NULL);

		dd = hci_open_dev(dev_id);
		err = hci_le_set_scan_parameters(dd, scan_type, interval, window, own_type, 0x00, 1000);
		err = hci_le_set_scan_enable(dd, 0x01, filter_dup, 1000);
		err = print_advertising_devices(dd, filter_type);
		err = hci_le_set_scan_enable(dd, 0x00, filter_dup, 1000);
		hci_close_dev(dd);
}


	else if(strcmp(argv[1],"-b") == 0)
	{
		opt_dst = (char *)malloc(18);
	strcpy(opt_dst,argv[2]);
		
	}
	else
	{
		printf("Bad option...\n");
		return 0;
	}
	
	
	
	
	opt_sec_level = (char *)malloc(4);
	strcpy(opt_sec_level,"low");

	if (opt_interactive) {
		interactive(opt_src, opt_dst, opt_psm);
	}
	else {
		chan = gatt_connect(opt_src, opt_dst, opt_sec_level,
					opt_psm, opt_mtu, connect_cb);
		if (chan == NULL) {
			got_error = 1;
		}
		if(!got_error) {
			event_loop = g_main_loop_new(NULL, FALSE);
			g_main_loop_run(event_loop);
			g_main_loop_unref(event_loop);
		}
	}

	//g_option_context_free(context);
	free(opt_src);
	free(opt_dst);
	free(opt_uuid);
	free(opt_sec_level);

	if (got_error)
		exit(EXIT_FAILURE);
	else
		exit(EXIT_SUCCESS);
}

