import SafeStrip_Python_mqtt as spm

import binascii

# sample data of CAM_message
sample_data = { 'hex_payload' : '000001723bb203c500000002000001915f4d0518b085f7fd33b2ee00000fff00000fff00000e1100012cc801c2000503ffff00007fffff020000a17fffff' , 'topic' : 'SafeStrip/CAM/401' }

payload = binascii.unhexlify(sample_data['hex_payload'])
topic   = sample_data['topic']

# knowing the type of the message
content1 = spm.CAM_message_decode( payload )

# type unknow - use the topic
content2 = spm.all_MQTT_decoding( topic , payload )

# knowing the type of the message
content3 = spm.CAM_message_decode_true_values( payload )

# type unknow - use the topic
content4 = spm.all_MQTT_decoding_true_values( topic , payload )

print(content1)
print(content2)
print(content3)
print(content4)
# pack to send

packed_lunch = spm.CAM_message_encode( content1 )
topic_lunch  = spm.CAM_message_MQTT_topic( content1 )

# unpack to verify consistency

content2 = spm.all_MQTT_decoding( topic_lunch , packed_lunch )

#print(packed_lunch)
#print(content2)

# TRY MAP/PARKING in order to verify vector arguments...

sample_data = { 'hex_payload' : '000001709ff5862700000005a028304000000000011b76da85069f05ce00000000000000000000000000010203000000000016eba67e0a00000000000000c054f27600d0d276000000000000000000000000ffffffffece6a67e0000000000000000000000000000000000000000000000000006962b7ea6ec740000000000000000000000000000000000000000000000007ea6eb770000000a00000005009f931f0000002076f254c076d2d000000000000000000100000000ffffffff7ea6e754000000000006962800000000000000010006962b7ea6ecd46575727476c40000000aef587ea6e7987ea6e8480000000100076dc0000aef587ea6e798000aef587ea6e848000000007ea6edc80006962b00000064a00000007ea6e84076c77b790000000976d2dc3876f254c07ea6e8480000000000076dc0000000627ea6e8286666666676d0b1cc76c7758876c77588000afcc87ea6e848000000000006962800000000000000000000000000000000000000000000000000000000000000007ea6ec640000000a0000181c009f8eea0000002076f254c076d2d000000000000000000100000000ffffffff7ea6e844000000000006962800000000000000040006962b7ea6edc430302e3142003000696220587972616e2e3830005a39313700001600000001480000000400000016000000040000000800000083000000010000000600001760000017600000000c000000000000000000000004000000000000007e00000001000000000000000080000000aaaaaaab000846e176d2dc387ea6e98876cae01100000089000000010000000676d2baac000000007ea6e988000000000000000000000004000000007ea6e9140000000176f318b476c5718976f2800076f31568000000007ea6ea4b00000000ff000418ffffffff76f280000000002076a4a4280000000076f0f499000000000000000000000004000000010000009d000000010000000076d2dc3876f254c07ea6ea380000000000000000000000000000000000000000000000007ea6e9887ea6ea380000000100076dc0000000640006274700000034000000210000000a000000030000000200000078000000020000003e000000667ea6ea186666666640747a66000000027ea6eff8000af2087ea6ea3800000000000656ed7ea6eaf47ea6eaf5000000007ea6ec787ea6e9fc000aef5800076dc07ea6f2c0000000000000020000084bac009f8f247ea6ea58000000049999999a3fd9999900a0aef000000000007370657600006276ed97c8000000007ea6ea687ea6eb1476edcf3476e91a1d30342e30003534003030363433333a002e32353a5a3030387ea6eb0076e9198d393031380000000000000000000000000000002076edd368000aef580000000476edcf34000af00076d10dd80000000176c9173976d2dcb8000000007ea6eb530000000176c90f857ea6eb1476edb00076c923c1000000010000000176d2dcb87ea6eb5376c91bdd76c91ac176d2dcb800000001000000017ea6eb530000000176f2500076c8a9bdc00800000000000076d2dcb876c89e5b000af0740000000000003002000af078000af07476e99a010000005b000aef587ea6eb5376edcf340000000076edcf01000af07476d2d0000000000176d2d000000af074009f890000001fad00000000009faeb476ce118f000aef58009faefc009faf1476c0dc39009faeb400001090009fae20009faefc009faf1476c0bd2d00000000009fae2000a0c590000aef58009faefc00001fad0000000010624dd37ea6fec876c0be5d00000000906fbf007ea6ec1c000aef5800000000009fae207ea6ec1c76c0a16b7ea6ec1c000000000000000000000000000b11c8000000007ea6ec5c0000003e000000000000906fec887ea68900009fec1c7ea6f2607ea60000000032905e5e00000000d34d6210c8fea67e43f80500c840cb280000000000000000d3f900050005000000007001f59fe88500000200000000001c181b05db76064f009f00e300000000bf00906f0e000011f190000af1b0000a473c000800007ff8ef58000a47380008f208000a473000085a3b0006f2c07ea6f3f00006001900002e110006e1a40006000a000000000000000000000000000000000000000000000000000046f40008000000000000000000000000000000000000000046f8000800030000f940000a0000000000000000000000000303000000000000000000000000000080000000000000000000000000000000004708000300000088fb0a000000000000000000000000000000000000000000000000000000000009000000000000000000000000000000044708000600000001020a000000000000000000000000000000000000000000000000000000f87f000000000000000000000000000000000447080002000000a8f10a000000000000000000000000000000000000000000000000000000f87f0000000000000000' , 'topic' : 'SafeStrip/MAPEM/-1607978944' }

payload = binascii.unhexlify(sample_data['hex_payload'])

contentMappo = spm.all_MQTT_decoding( sample_data['topic'] , payload )

#print(contentMappo)