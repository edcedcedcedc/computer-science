select nameGiven, teamID, count(distinct awardsplayers.yearID) as yearID_distinct from people
  inner join appearances on
  appearances.playerID = people.playerID
  inner join leagues on 
  leagues.lgID = appearances.lgID
  inner join awardsplayers on
  awardsplayers.playerID = people.playerID and awardsplayers.yearID = appearances.yearID
  where awardID = 'Gold Glove' and 
  leagues.active = 'Y' and 
  awardsplayers.yearID > 1999 and 
  appearances.G_batting > (
  select avg(a2.G_batting) from appearances a2
  where a2.teamID = appearances.teamID and 
  a2.yearID > 1999)
  group by nameGiven, teamID, people.playerID
  order by yearID_distinct desc, nameGiven asc
  limit 10;