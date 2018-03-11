CC=g++
CFLAGS=-std=c++17
#DEFINES=DEBUG_MODE
#DEFINESTINY=TIXML_USE_STL
DEFINESTINY=TIXML

INCLUDECADMIUM=-I ../cadmium/include

all: ../data_structures/communication.o ../data_structures/nep_model_enum_types.o ../data_structures/struct_types.o ../data_structures/decision_maker_behaviour_enum_types.o Mainmodel.o ../tinyXML/tinyxml.o ../tinyXML/tinyxmlerror.o ../tinyXML/tinyxmlparser.o ../tinyXML/tinystr.o
	$(CC) -o executables/TOPMODEL.out Mainmodel.o ../data_structures/communication.o ../data_structures/nep_model_enum_types.o ../data_structures/struct_types.o ../data_structures/decision_maker_behaviour_enum_types.o ../tinyXML/tinyxml.o ../tinyXML/tinyxmlerror.o ../tinyXML/tinyxmlparser.o ../tinyXML/tinystr.o

Mainmodel.o: Mainmodel.cpp
	time $(CC) -c $(CFLAGS) $(INCLUDECADMIUM) Mainmodel.cpp -o Mainmodel.o

../data_structures/communication.o: 
	time $(CC) -c $(CFLAGS) ../data_structures/communication.cpp -o ../data_structures/communication.o 

../data_structures/nep_model_enum_types.o: 
	time $(CC) -c $(CFLAGS) ../data_structures/nep_model_enum_types.cpp -o ../data_structures/nep_model_enum_types.o

../data_structures/struct_types.o: 
	time $(CC) -c $(CFLAGS) ../data_structures/struct_types.cpp -o ../data_structures/struct_types.o

../data_structures/decision_maker_behaviour_enum_types.o: 
	time $(CC) -c $(CFLAGS) ../data_structures/decision_maker_behaviour_enum_types.cpp -o ../data_structures/decision_maker_behaviour_enum_types.o

../tinyXML/tinyxml.o: ../tinyXML/tinyxml.cpp
	time $(CC) -D$(DEFINESTINY) -c $(CFLAGS) ../tinyXML/tinyxml.cpp -o ../tinyXML/tinyxml.o

../tinyXML/tinyxmlerror.o: ../tinyXML/tinyxmlerror.cpp
	time $(CC) -D$(DEFINESTINY) -c $(CFLAGS)  ../tinyXML/tinyxmlerror.cpp -o ../tinyXML/tinyxmlerror.o

../tinyXML/tinyxmlparser.o: ../tinyXML/tinyxmlparser.cpp 
	time $(CC) -D$(DEFINESTINY) -c $(CFLAGS)  ../tinyXML/tinyxmlparser.cpp -o ../tinyXML/tinyxmlparser.o

../tinyXML/tinystr.o: ../tinyXML/tinystr.cpp
	time $(CC) -D$(DEFINESTINY) -c $(CFLAGS)  ../tinyXML/tinystr.cpp -o ../tinyXML/tinystr.o

clean:
	-for d in executables; do (cd $$d; rm -f TOPMODEL.out *~ ); done
	-for d in ../data_structures; do (cd $$d; rm -f *.o *~ ); done
	-for d in ../tinyXML; do (cd $$d; rm -f *.o *~ ); done