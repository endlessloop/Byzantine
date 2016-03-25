#Byzantine Generals Problem

Byzantine
Generals Problem states that, for N generals, taking part to reach a consensus, have M traitors
among them. Write the application such that, an arbitrarily picked commanding general can
send an order to lieutenant generals such that:  
(a) If the commanding general is loyal, then every loyal lieutenant obeys the order he sends.  
(b) All loyal generals obey the same order

**Command to build on commandline:**  
g++ Byzantine.cpp -o Byzantine  

**Command to run:**  
./Byzantine

**VC++ download Link:**  
https://www.microsoft.com/en-in/download/details.aspx?id=5555

**Credits**  
http://marknelson.us/2007/07/23/byzantine/


**Sample Output:**  

Enter number of Generals: 5  
Enter number of Traitors: 1   

Source = 2  
Children = 20,21,23,24  

================================== Messages =====================================  
Sending from process 2 to 0: {1, 2, ?}  
Sending from process 2 to 1: {1, 2, ?}  
Sending from process 2 to 3: {1, 2, ?}  
Sending from process 2 to 4: {?, 2, ?}  

Sending from process 0 to 0: {1, 20, ?}  
Sending from process 0 to 1: {1, 20, ?}  
Sending from process 0 to 3: {1, 20, ?}  
Sending from process 0 to 4: {?, 20, ?}  

Sending from process 1 to 0: {1, 21, ?}  
Sending from process 1 to 1: {1, 21, ?}  
Sending from process 1 to 3: {1, 21, ?}  
Sending from process 1 to 4: {?, 21, ?}  

Sending from process 3 to 0: {1, 23, ?}  
Sending from process 3 to 1: {1, 23, ?}  
Sending from process 3 to 3: {1, 23, ?}  
Sending from process 3 to 4: {?, 23, ?}  

Sending from process 4 to 0: {?, 24, ?}  
Sending from process 4 to 1: {?, 24, ?}  
Sending from process 4 to 3: {?, 24, ?}  
Sending from process 4 to 4: {?, 24, ?}  

Process 0 decides on value 1  
//--------------------------------------------  
{1,20,?}  
{1,21,?}  
{1,23,?}  
{?,24,?}  
{1,2,?}  

Process 1 decides on value 1  
//-------------------------------------------- 
{1,20,1}  
{1,21,1}  
{1,23,1}  
{?,24,?}  
{1,2,1}  

Source Process 2 decides on value 1  
//-------------------------------------------- 
{1,20,1}  
{1,21,1}  
{1,23,1}  
{?,24,?}  
{1,2,1}  

Process 3 decides on value 1  
//-------------------------------------------- 
{1,20,1}  
{1,21,1}  
{1,23,1}  
{?,24,?}  
{1,2,1}  

Process 4 is faulty
//-------------------------------------------- 
{1,20,1}  
{1,21,1}  
{1,23,1}  
{?,24,?}  
{1,2,1}    

================================== Consensus =====================================  
CONSENSUS REACHED!
