-- I divided the problem into subproblems 


-- Consider the event E: for a given (team, year), the team has more than 5 distinct players 
/* 
select teams.teamID, teams.yearID, count(distinct people.playerID) as distinct_player_id from people
· inner join appearances on
· appearances.playerID = people.playerID
· inner join teams on 
· teams.teamID = appearances.teamID
‣ group by teams.teamID, teams.yearID; */

-- win an award

/* 
select teams.teamID, teams.yearID, count(distinct people.playerID) as distinct_player_id from people
  inner join appearances on
  appearances.playerID = people.playerID
  inner join teams on 
  teams.teamID = appearances.teamID
  inner join awardsplayers on
  awardsplayers.yearID = teams.yearID and awardsplayers.playerID = people.playerID
  group by teams.teamID, teams.yearID
  limit 10; */


  -- and some manager has won an award in the same year.
  -- COMMENT: I also added having to match > 5 after aggregation 

/* 
select teams.teamID, teams.yearID, count(distinct people.playerID) as distinct_player_id from people
inner join appearances on
appearances.playerID = people.playerID
inner join teams on 
teams.teamID = appearances.teamID and teams.yearID = appearances.yearID
inner join awardsplayers on
awardsplayers.yearID = teams.yearID and awardsplayers.playerID = people.playerID
where exists (select 1 from awardsmanagers where awardsmanagers.yearID = teams.yearID)
group by teams.teamID, teams.yearID
having count(distinct people.playerID) > 5
limit 5;
 */

-- For all active leagues, find the teams where the event E has happened more than once.

/* 
WITH event_e AS (
    -- Put your working inner query right here inside the parentheses
)
SELECT 
    -- What columns do you want to show in your final output?
    -- (Hint: You need league name, team name, and a count of the years)
FROM event_e
-- Join back to teams and leagues here so you can get the readable names
-- Add your filter for active leagues ('Y')
-- Group by the names
-- Filter for teams where the count is greater than 1 using HAVING
-- Order by count (most to least) and team name alphabetically
 */
with event_e as (
select teams.teamID, teams.yearID, teams.lgID  from people
inner join appearances on
appearances.playerID = people.playerID
inner join teams on 
teams.teamID = appearances.teamID and teams.yearID = appearances.yearID
inner join awardsplayers on
awardsplayers.yearID = teams.yearID and awardsplayers.playerID = people.playerID
where exists (select 1 from awardsmanagers where awardsmanagers.yearID = teams.yearID)
group by teams.teamID, teams.yearID, teams.lgID
having count(distinct people.playerID) > 5)
select leagues.league as league, teams.name as teams_name,  count(distinct event_e.yearID) as distinct_years from event_e
inner join teams on
teams.teamID = event_e.teamID and teams.yearID = event_e.yearID
inner join leagues on
leagues.lgID = event_e.lgID
where leagues.active = 'Y' 
group by leagues.league, teams.name
having count(distinct event_e.yearID) > 1
order by distinct_years desc, teams.name asc;