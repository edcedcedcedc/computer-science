""" 

WARM UP 04/10/2026 morning // hash map 

 """

def seen_twice(s: str) -> str:
    map = dict()
    for _, v in enumerate(s):
        frequency = map.get(v, 0) + 1
        if frequency >= 2:
            return v 
    return None 


""" 

WARM UP CHALLENGE // hash map 

 """

def first_non_repeating(s:str) -> int:
    map = dict()  
    for i, v in enumerate(s):
        obj = map.get(v, {"frequency": 0, "index": i})
        obj["frequency"] += 1
    for k, v in map.items():
        if v["frequency"] < 2:
            return v["index"] 
    return -1


""" 

REVIEW // hash map, slightly different problem same concept 

 """


    



""" 
MASTERY CHALLENGE // variable-size sliding window/hash map 
    complexity of this solution is O(n)
 """

def longest_substring_without_repeating(s: str) -> str:
    left_pointer = 0
    right_pointer = 0
    current_len = 0
    max_len = 0
    l = list(s)
    map = dict()

    while right_pointer <= len(l) - 1:
            if l[right_pointer] in map:
                if map[l[right_pointer]] >= left_pointer:
                    left_pointer = map[l[right_pointer]] + 1              
            current_len = right_pointer - left_pointer + 1
            if current_len > max_len:
                max_len = current_len
            map[l[right_pointer]] = right_pointer     
            right_pointer += 1
            
    return max_len


test_cases = [
    ("", 0),
    ("abcdefg", 7),
    ("aaa", 1),
    ("abcab", 3)
]
def run_test_suite(test_cases: list[tuple[str, int]]) -> str:
    for i in range(len(test_cases)):
        result = longest_substring_without_repeating(test_cases[i][0])
        if result == test_cases[i][1]:
            print(f"PASSED for {test_cases[i][1]} == {result}")
        else:
            print(f"FAILED for {test_cases[i][1]} == {result}")

run_test_suite(test_cases)