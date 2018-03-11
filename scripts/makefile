CC=g++
CFLAGS=-std=c++17
#DEFINESTINY=TIXML_USE_STL
DEFINESTINY=TIXML

all: generateDevicesBroken.out generateTOPMODEL.out

generateTOPMODEL.out: generateTOPMODEL.o ../data_structures/nep_model_enum_types.o ../data_structures/decision_maker_behaviour_enum_types.o ../tinyXML/tinyxml.o ../tinyXML/tinyxmlerror.o ../tinyXML/tinyxmlparser.o ../tinyXML/tinystr.o
	$(CC) -g -o generateTOPMODEL.out generateTOPMODEL.o ../data_structures/nep_model_enum_types.o ../data_structures/decision_maker_behaviour_enum_types.o ../tinyXML/tinyxml.o ../tinyXML/tinyxmlerror.o ../tinyXML/tinyxmlparser.o ../tinyXML/tinystr.o

generateTOPMODEL.o:
	$(CC) -g -c $(CFLAGS) generateTOPMODEL.cpp -o generateTOPMODEL.o

generateDevicesBroken.out: generateDevicesBroken.o ../data_structures/nep_model_enum_types.o ../data_structures/decision_maker_behaviour_enum_types.o ../tinyXML/tinyxml.o ../tinyXML/tinyxmlerror.o ../tinyXML/tinyxmlparser.o ../tinyXML/tinystr.o
	$(CC) -g -o generateDevicesBroken.out generateDevicesBroken.o ../data_structures/nep_model_enum_types.o ../data_structures/decision_maker_behaviour_enum_types.o ../tinyXML/tinyxml.o ../tinyXML/tinyxmlerror.o ../tinyXML/tinyxmlparser.o ../tinyXML/tinystr.o

generateDevicesBroken.o:
	$(CC) -g -c $(CFLAGS) generateDevicesBroken.cpp -o generateDevicesBroken.o

../data_structures/nep_model_enum_types.o: 
	$(CC) -g -c $(CFLAGS) ../data_structures/nep_model_enum_types.cpp -o ../data_structures/nep_model_enum_types.o

../data_structures/decision_maker_behaviour_enum_types.o: 
	$(CC) -g -c $(CFLAGS) ../data_structures/decision_maker_behaviour_enum_types.cpp -o ../data_structures/decision_maker_behaviour_enum_types.o

../tinyXML/tinyxml.o: ../tinyXML/tinyxml.cpp
	$(CC) -D$(DEFINESTINY) -g -c $(CFLAGS) ../tinyXML/tinyxml.cpp -o ../tinyXML/tinyxml.o

../tinyXML/tinyxmlerror.o: ../tinyXML/tinyxmlerror.cpp
	$(CC) -D$(DEFINESTINY) -g -c $(CFLAGS) ../tinyXML/tinyxmlerror.cpp -o ../tinyXML/tinyxmlerror.o

../tinyXML/tinyxmlparser.o: ../tinyXML/tinyxmlparser.cpp 
	$(CC) -D$(DEFINESTINY) -g -c $(CFLAGS) ../tinyXML/tinyxmlparser.cpp -o ../tinyXML/tinyxmlparser.o

../tinyXML/tinystr.o: ../tinyXML/tinystr.cpp
	$(CC) -D$(DEFINESTINY) -g -c $(CFLAGS) ../tinyXML/tinystr.cpp -o ../tinyXML/tinystr.o

clean:
	rm -f *.out *.o *~
	-for d in ../data_structures; do (cd $$d; rm -f *.o *~ ); done
	-for d in ../tinyXML; do (cd $$d; rm -f *.o *~ ); done
