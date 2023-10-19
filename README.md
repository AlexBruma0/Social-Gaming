# Social Gaming

## Team members:
<ol>
  <li>Kevin Chen</li>
  <li>Caleb Tong</li>
  <li>Arjun Singh-Shokar</li>
  <li>Alex Bruma</li>
  <li>Lex Rohweder</li>
  <li>Zelin Zhu</li>
  <li>Evan Mangat</li>
</ol>

## How to use
- Create a build directory out of the source and make it
- Resources folder contains things like game files or html
    - These are copied over by the custom command in the CMakeLists.txt in tests/networking and tests/parser
    - Only the games fodlers and networking folders in resources are copied over
- You might have to install GTest library 
    - Tests are executables will be located in the tests build/tests/(networking or parser)
    - The tests are only meant to test the libraries
- Bin folder will contain our project executables so stuff that we want to show and deploy
    - Right now it only has the chat server

## Networking
- If you want to change the client type go into tools/CMakeLists.txt and change the add_subdirectory to the client you want
- To run the chat server do ./bin/chatserver 8000 resources/networking/webchat.html
    - Resources will have the webchat.html
- If you want to change the html add a file to the resources networking folder 

## Parser
- If you want to add files add a new file but make sure to add the .cpp to the add_library in lib/parser/CMakeLists.txt
- Right now parser.h is generic and you can change it
- Remember to include any new files you create in the header of the parser_test
