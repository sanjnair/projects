package barqsoft.footballscores;
import android.content.Context;

import java.util.Hashtable;

/**
 * Created by yehya khaled on 3/3/2015.
 */
public class Utilies
{
    // Moved from MyFetchService
    // This set of league codes is for the 2015/2016 season. In fall of 2016, they will need to
    // be updated. Feel free to use the codes
    public static final String BUNDESLIGA1         = "394";
    public static final String BUNDESLIGA2         = "395";
    public static final String LIGUE1              = "396";
    public static final String LIGUE2              = "397";
    public static final String PREMIER_LEAGUE      = "398";
    public static final String PRIMERA_DIVISION    = "399";
    public static final String SEGUNDA_DIVISION    = "400";
    public static final String SERIE_A             = "401";
    public static final String PRIMERA_LIGA        = "402";
    public static final String BUNDESLIGA3         = "403";
    public static final String EREDIVISIE          = "404";
    public static final String CHAMPIONS_LEAGUE    = "405";

    public static final String INTENT_ACTION_SCORE_UPDATED = "barqsoft.footballscores.service.SCORES_UPDATED";

    //
    // Static hashtable to keep the team number with names
    //
    private static final Hashtable<String, String> LeagueTable;
    private static final Hashtable<String, Integer> CrestTable;

    //
    // Creates the league table
    //
    static {
        LeagueTable = new Hashtable<String, String>();

        LeagueTable.put(BUNDESLIGA1,        "BUNDESLIGA1-1");
        LeagueTable.put(BUNDESLIGA2,        "BUNDESLIGA-2");
        LeagueTable.put(LIGUE1,             "LIGUE-1");
        LeagueTable.put(LIGUE2,             "LIGUE-2");
        LeagueTable.put(PREMIER_LEAGUE,     "PREMIER LEAGUE");
        LeagueTable.put(PRIMERA_DIVISION,   "PRIMERA DIVISION");
        LeagueTable.put(SEGUNDA_DIVISION,   "SEGUNDA DIVISION");
        LeagueTable.put(SERIE_A,            "SERIE A");
        LeagueTable.put(PRIMERA_LIGA,       "PRIMERA LIGA");
        LeagueTable.put(BUNDESLIGA3,        "BUNDESLIGA-3");
        LeagueTable.put(EREDIVISIE,         "EREDIVISIE");
        LeagueTable.put(CHAMPIONS_LEAGUE,   "CHAMPIONS_LEAGUE");
    }

    //
    // Creates Team Crest table
    //
    static
    {
        CrestTable = new Hashtable<String, Integer>();

        CrestTable.put("arsenal fc",                    R.drawable.arsenal);
        CrestTable.put("aston villa fc",                R.drawable.aston_villa);
        CrestTable.put("chelsea fc",                    R.drawable.chelsea);
        CrestTable.put("crystal palace fc",             R.drawable.crystal_palace_fc);
        CrestTable.put("everton fc",                    R.drawable.everton_fc_logo1);
        CrestTable.put("leicester city fc",             R.drawable.leicester_city_fc_hd_logo);
        CrestTable.put("liverpool fc",                  R.drawable.liverpool);
        CrestTable.put("manchester city fc",            R.drawable.manchester_city);
        CrestTable.put("manchester united fc",          R.drawable.manchester_united);
        CrestTable.put("newcastle united fc",           R.drawable.newcastle_united);
        CrestTable.put("southampton fc",                R.drawable.southampton_fc);
        CrestTable.put("stoke city fc",                 R.drawable.stoke_city);
        CrestTable.put("sunderland afc",                R.drawable.sunderland);
        CrestTable.put("swansea city fc",               R.drawable.swansea_city_afc);
        CrestTable.put("tottenham hotspur fc",          R.drawable.tottenham_hotspur);
        CrestTable.put("west bromwich albion fc",       R.drawable.west_bromwich_albion_hd_logo);
        CrestTable.put("west ham united fc",            R.drawable.west_ham);
    }

    //
    // Returns the League
    //
    public static String getLeague(String id) {
        if (LeagueTable.containsKey(id)) {
            return LeagueTable.get(id);
        }

        return "Not known League Please report";
    }

    //
    // Returns true if League is a known league
    //
    public static boolean isKnownLeaguue(String id)
    {
        return LeagueTable.containsKey(id);
    }

    public static String getMatchDay(int match_day, String league_num)
    {
        if(league_num.equals(CHAMPIONS_LEAGUE))
        {
            if (match_day <= 6)
            {
                return "Group Stages, Matchday : 6";
            }
            else if(match_day == 7 || match_day == 8)
            {
                return "First Knockout round";
            }
            else if(match_day == 9 || match_day == 10)
            {
                return "QuarterFinal";
            }
            else if(match_day == 11 || match_day == 12)
            {
                return "SemiFinal";
            }
            else
            {
                return "Final";
            }
        }
        else
        {
            return "Matchday : " + String.valueOf(match_day);
        }
    }

    public static String getScores(int home_goals,int awaygoals)
    {
        if(home_goals < 0 || awaygoals < 0)
        {
            return " - ";
        }
        else
        {
            return String.valueOf(home_goals) + " - " + String.valueOf(awaygoals);
        }
    }

    //
    // Returns Team crest
    //
    public static int getTeamCrestByTeamName (String teamname)
    {
        if (teamname==null){return R.drawable.no_icon;}

        String key = teamname.toLowerCase();

        if (CrestTable.containsKey(key))
        {
            return CrestTable.get(key).intValue();
        }
        else
        {
            return R.drawable.no_icon;
        }
    }
}
