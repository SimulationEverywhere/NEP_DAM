CC=g++
CFLAGS=-std=c++14
DEFINES=DEBUG_MODE
DEFINESTINY=TIXML_USE_STL
INCLUDECADMIUM=-I ../../../cadmium/include

all: ../../../data_structures/nep_model_enum_types.o ../../../data_structures/decision_maker_behaviour_enum_types.o ../../../data_structures/struct_types.o ../../../data_structures/communication.o ../../../tinyXML/tinyxml.o main.o ../../../tinyXML/tinyxmlerror.o ../../../tinyXML/tinyxmlparser.o ../../../tinyXML/tinystr.o
	$(CC) -g -o DecisionMaker.out main.o ../../../data_structures/nep_model_enum_types.o ../../../data_structures/decision_maker_behaviour_enum_types.o ../../../data_structures/struct_types.o ../../../data_structures/communication.o ../../../tinyXML/tinyxml.o ../../../tinyXML/tinyxmlerror.o ../../../tinyXML/tinyxmlparser.o ../../../tinyXML/tinystr.o

main.o: main.cpp
	$(CC) -D$(DEFINESTINY) -D$(DEFINES) -g -c $(CFLAGS) $(INCLUDECADMIUM) main.cpp -o main.o

../../../data_structures/decision_maker_behaviour_enum_types.o: 
	$(CC) -g -c $(CFLAGS)  ../../../data_structures/decision_maker_behaviour_enum_types.cpp -o ../../../data_structures/decision_maker_behaviour_enum_types.o

#data_structures
../../../data_structures/nep_model_enum_types.o: 
	$(CC) -g -c $(CFLAGS) ../../../data_structures/nep_model_enum_types.cpp -o ../../../data_structures/nep_model_enum_types.o

../../../data_structures/communication.o: 
	$(CC) -g -c $(CFLAGS) ../../../data_structures/communication.cpp -o ../../../data_structures/communication.o

../../../data_structures/struct_types.o: 
	$(CC) -g -c $(CFLAGS) ../../../data_structures/struct_types.cpp -o ../../../data_structures/struct_types.o

#tinyXML
../../../tinyXML/tinyxml.o: ../../../tinyXML/tinyxml.cpp
	$(CC) -D$(DEFINESTINY) -c ../../../tinyXML/tinyxml.cpp -o ../../../tinyXML/tinyxml.o

../../../tinyXML/tinyxmlerror.o: ../../../tinyXML/tinyxmlerror.cpp
	$(CC) -D$(DEFINESTINY) -g -c ../../../tinyXML/tinyxmlerror.cpp -o ../../../tinyXML/tinyxmlerror.o

../../../tinyXML/tinyxmlparser.o: ../../../tinyXML/tinyxmlparser.cpp 
	$(CC) -D$(DEFINESTINY) -g -c ../../../tinyXML/tinyxmlparser.cpp -o ../../../tinyXML/tinyxmlparser.o

../../../tinyXML/tinystr.o: ../../../tinyXML/tinystr.cpp
	$(CC) -D$(DEFINESTINY) -g -c ../../../tinyXML/tinystr.cpp -o ../../../tinyXML/tinystr.o

clean:
	rm -f DecisionMaker.out *.o *~
	-for d in ../; do (cd $$d; rm -f *.o *~ ); done
	-for d in ../../../data_structures; do (cd $$d; rm -f *.o *~ ); done
	-for d in ../../../tinyXML; do (cd $$d; rm -f *.o *~ ); done

