package com.xmatrix;
import java.util.Random;

public class JokeTeller {

    private static final String[] JOKES_LIST = {
            "(Q) Why do programmers always mix up Halloween and Christmas? (A) Because Oct 31 == Dec 25",
            "(Q) What is the Object Oriented way to become wealthy? (A) Inheritance",
            "(Q) How many programmers does it take to change a light bulb? (A) None – It’s a hardware problem",
            "(Q) What's the best part about TCP jokes? (A) I get to keep telling them until you get them.",
            "(Q) Why do java programmers have to wear glasses? (A) Because they don't C# (see sharp)",
            "(Q) What is a programmers favorite hangout place? (A) Foo Bar",
            "(Q) Definition of an upgrade? (A) Take old bugs out, put new ones in",
            "(Q) Why are PCs like air conditioners? (A) They stop working properly if you open Windows!",
            "(Q) How do you make your Mac go faster? (A) Drop it from a higher window.",
            "(Q) What do you call a bent iPhone 6 plus? (A) A dead wringer.",
            "(Q) Why do they call it hyper text? (A) Too much JAVA!",
            "There are 10 types of people in the world: those who understand binary, and those who don't.",
            "In order to understand recursion you must first understand recursion.",
            "I would tell you a UDP joke, but you might not get it",
            "There's a band called 1023MB. They haven't had any gigs yet",
            "Knock, knock. Who's there? very long pause........ Java."
            };

    private static final Random m_rand = new Random();

    /**
     * Constructor is private
     */
    private JokeTeller()
    {
    }

    //
    // Return a random joke
    //
    public static String getJoke()
    {
        int idx = m_rand.nextInt(JOKES_LIST.length);
        return  JOKES_LIST[idx];
    }
}
