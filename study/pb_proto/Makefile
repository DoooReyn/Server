all: proto_msg write reader

clean:
	rm -f write reader msg.*.cc msg.*.h *.o log

proto_msg:
	sh ./gen_proto.sh


write: ./gen_proto/msg.pb.cc write.cc
	g++  ./gen_proto/msg.pb.cc write.cc -o write  `pkg-config --cflags --libs protobuf`

reader: ./gen_proto/msg.pb.cc reader.cc
	g++  ./gen_proto/msg.pb.cc reader.cc -o reader  `pkg-config --cflags --libs protobuf`
