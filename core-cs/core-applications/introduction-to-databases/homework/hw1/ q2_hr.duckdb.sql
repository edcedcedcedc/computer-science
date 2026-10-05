select nameFirst || ' (' || nameGiven || ') ' || nameLast as name, max(HR) as max_hr from people
· inner join appearances on 
· appearances.playerID = people.playerID
· inner join collegeplaying on
· collegeplaying.playerID = people.playerID
· inner join schools on 
· schools.schoolID = collegeplaying.schoolID
· where schools.state = 'PA'
· group by appearances.playerID, nameFirst, nameGiven, nameLast
· order by max_hr desc, nameFirst asc
‣ limit 10;