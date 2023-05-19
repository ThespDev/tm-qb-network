# tm-qb-network
A programming quiz which utilises networks.

## QB
QB summoned with command: <br>
`./qb <IP-Address> <.CSV File> <language>`
* `<IP Address>` ~ *IP Address of TM Server*
* `<.CSV File>` ~ *CSV file which contains question information, see below for formatting*
* `<Language>` ~ *Language for the qb server to run on, only 2 can be executed, java or c (this value must be in lower case)*

### Format for QB CSV files:
-All data for QB stored in CSV Files <br>
-Each line represents a new Question <br>
-Commas cannot be used anywhere in question but to delinate different fields
<br>
**Line format:**
<br>
`<Question Type>,<Question Number>,<Question Body>,<Question Answer/Expected Results from Inputs>,<Answer (MCA Only)`
<br>
#### Question type
Only 2 Variables can be used, either:  <br>

- **MCA** ~ Multiple Choice A(? Meaning of A Lost to time) <br>
  - *Essentially Multiple Choice Questions*br>

- **Code** ~ Code Question <br>
  - *Question to be compiled and executed.*

#### Question Number 
*Mostly for Formatting and readability not essential for any functionality* 

#### Question Body
*Body of text for Questions, what will be shown to student*

#### Question Answers/Expected Results
*Differences based on Type of Question* <br>
- `MCA` - Different choices for Answer, delinated by > <br>
- `Cde` - Inputs and expected outputs delinate by > <br>
   - First 3 > are inputs, last 3 > are expected outputs (to be marked as correct)

#### Answer (MCA Only) 
*Actual Answer for MCA Question, each number represents multiple choice, starts 1 - 4 (not 0)* <br>
*Program can only store maximum of 4 Questions*

### examples: <br> 
`MCA,1,Which of the following is not a valid data type in C?,>int>string>float>char,2`<br>
So in this case the correct answer would be the second option (string) <br>
`Code,7,Write Code to output and argument as a string,>hello>test>argumenttest>hello>test>argumenttest` <br>
In this case the first 3 options (hello,test,argumenttest) will be what's added to compiled code when summoned
And the last 3 (hello,test,argumenttest) are the expected outputs

## TM
