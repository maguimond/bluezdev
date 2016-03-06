''Public Functions'':
errorData Connect(server_mac_addr)
errorData Connect(server_name)
errorData Disconnect(local_dev_id)
errorData AppendCheksumToData(*data)
errorData Write(hnd, data)				:	Wait for callback, and check ACK flag, (checksum?)
errorData Read(hnd, *data)				:	Wait for callback, verify checksum, (ACK flag?)

''Private Functions'':
errorCode ComputeChecksum(data, *checksum)
errorCode ConnectCallback(...)
errorCode WriteCallBack(...)
errorCode ReadCallBack(...)


Usage Note:
This is not a service, there is no main, it can only be used as a ''library''. 
It offers interace functions that can be used by a service or a tool; another program.


Errors Define:
''Lower level'':
ENODEV									:	No local bluetooth device as been found
ENOSERVER								:	The specified server is not reachable
ENACKRECEIVED							:	Received a NACK from the server
ECHECKSUMMISMATCH						:	The localy computed checksum is different than the one in the data received
EBUFFEROVERFLOW							:	User tried to append checksum to a non-allocated pointer, or a badly allocated pointer.
''Higher level'' (Are caused by lower level error(s)):
ECONNECTFAILED							:	Connection Failed
EDISCONNECTFAILED						:	Disconection Failed
EAPPENDCHECKSUMFAILED					:	AppendChecksum Failed
EWRITEFAILED							:	Write Failed
EREADFAILED								:	Read Failed