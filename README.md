## tm-qb-network
A programming quiz which utilises networks.

### QB
Format for Data Storage:
-All data stored in CSV Files
-Each line represents a new Question

Line format:
`<Question Type>,<Question Number>,<Question Body>,<Question Answer/Expected Results from Inputs>,<Answer (MCA Only)`

Question type ~ 1 of 2 Variables
                MCA - Multiple Choice A(? Meaning of A Lost to time)
                      Essentially Multiple Choice Question
                Cde - Code Question
                      Question to be compiled and executed.
Question Number ~ Mostly for Formatting and readability not essential
Question Body ~ Body of text for Questions, what will be shown to student
Question Answers/Expected Results ~ Differences based on Type of Question
    MCA - Different choices for Answer, delinated by >
    Cde - Inputs and expected outputs delinate by >
          First 3 > are inputs, last 3 > are expected outputs (to be marked as correct)
Answer (MCA Only) ~ Actual Answer for MCA Question, each number represents multiple choice, starts 1 - 4 (not 0)
