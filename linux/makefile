CXX = g++
CXXFLAGS = -O3 -Wno-deprecated

%.o:           	%.cpp
		$(CXX) $(CXXFLAGS) -c $*.cpp 

mtcd_obj = 	mtcd.o model.o SpecialFunctions.o

mtcd:       	$(mtcd_obj)
		$(CXX) $(CXXFLAGS)   -o $@ $(mtcd_obj) -L. -lm -lstdc++
		chmod a+x mtcd
clean:		
		rm *.o
		rm mtcd

