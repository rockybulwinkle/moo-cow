~~~How to use this cow pie~~~

1. mkfifo in /wiiuse
2. build wii_capture with wiiuse/build.sh
3. capture basic up, down, left, right motions
	3a. wii_capture left
	3b. wii_capture right
	3c. wii_capture up
	3d. wii_capture down
4. run /make_training_file.py to generate a training file for the first layer
5. run /fann/build.sh to build the ann trainers
6. run cneural in its own directory (cd into fann first)
7. (optional) test the first layer by running wii_capture without any arguments, and run_network in /
8. run wiicapture like so.  Make a clockwise circle motion with the wii remote for the first function, and a counterclockwise motion for the second
	8a. wiicapture cwcircle 1
	8b. wiicapture ccwcircle 1
9. run run_network:
	9a. run_network cwcircle
	9b. run_network ccwcircle
10. run complex_make_training_file.py
11. cd into /fann and run second_layer
12. spin the wheel of deities and pray.
