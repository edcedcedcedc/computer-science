#Foundations (Arrays, Hash Maps, Strings)

#first duplicate value 
""" 
HWNDE

what I have 
a string

what I want

a string
first occurence of same letter

what I need 
set()

what is the edge case
nothing zero
no match 
single item 


what I do
assign string to a variable 
assign set to a variable 
if single item or nothing or zero or no duplicates
    return "1"
create a loop iterate thru the string 
if item not in set 
add item to set 
else 
return the item 
"""
def first_dup(s):
    my_set = set()
    if not s.isalpha() or len(s) == 1:
        return "1"
    for char in s:
        if char not in my_set:
            my_set.add(char)
        else:
            return char
    return "1"
print(first_dup("abcc"))

def first_dup_gpthelp(s):
    seen = set()
    if len(s) <= 1: # a string len 0 or 1 cannot have duplicates and excludes more subtle edge-cases 
        return None # None instead of 1
    for char in s:
        if char not in seen: #better semantics on naming seen instead of my_set
            seen.add(char)
        else:
            return char
    return None
print(first_dup("abcc"))