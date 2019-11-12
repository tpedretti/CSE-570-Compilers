                      *****************************
                         CS570/670 -- Spring 2017

                                 LAB1

                         Instructor: Ernesto Gomez
                      ******************************
DFAs are specified by a set of states (among them one starting and one 
or more accepting), and transitions between pairs of states which are 
labeled by a symbol of some alphabet. DFAs are typically given in one 
of two formats: 
(1) a transition diagram (that is, a directed graph), and
(2) a transition table. 
Both representations are equivalant to each other.  

As an example of a DFA, take a look in Purple dragon book on p. 151, Fig. 
3.28, a transition diagram. The DFA algorithm is descrived in pages 148-151. 


EXERCISE: Write a program that simulates the above DFA 
	in a language of your choice, but limited to something that
	could run on the machines in our labs. 
	
	  Accepts L=(a|b)*abb

	IT IS STRONGLY RECOMMENDED THAT YOU USE A TABLE DRIVEN
	APPROACH TO WRITING YOUR DFA.
	  
          A good program will be one that is easily modified
          to handle other transition diagrams.

          The input to the DFA should be a string; in the course
          of processing this string character-by-character, the
          DFA will undergo the specified state transitions. The
	  DFA accepts the string if it is an accepting state
	  when it has consumed its input; otherwise it rejects
	  the string.

DELIVERABLES:
	  Program source code, with internal documentation
	  Test results : use strings that are in L, and strings
	  not in L.

With thanks to: Kerstin Voigt                
