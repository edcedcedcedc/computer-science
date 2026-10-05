import test_suite

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


""" test_cases = [
    ("", 0),
    ("abcdefg", 7),
    ("aaa", 1),
    ("abcab", 3)
] """

""" 

WARM UP 05/10/2026 morning // variable size sliding window // hash map 

 """


def longest_substring_without_repeating_05102026(s: str) -> int:
    map = dict()
    left = 0
    right = 0
    l = list(s)
    current_size = 0
    maximum_size = 0
    while right <= len(l) - 1:
        if l[right] in map:
            if map[l[right]] >= left:
                left = map[l[right]] + 1 
        current_size = right - left + 1
        if current_size > maximum_size:
            maximum_size = current_size
        map[l[right]] = right
        right += 1
    return maximum_size                 

""" 

REVIEW 05/10/2026 morning // sliding window(variation problem) 

 """
test_cases = [
    ([7], 1),
    ([3,4,7], 1),
    ([3,4,1,2,4], 2)
]

#minimal len subarray whose sum is greater or equal to target 
def minimal_len_subarray_sum_05102026(l: list[int], t: int) -> int:
    left = 0
    right = 0
    curr_sum = 0
    curr_len = 0
    min_len = float('inf')
    while right <= len(l) - 1:
        if right >= left:
            curr_sum += l[right]
            while curr_sum >= t:
                curr_len = right - left + 1 
                if curr_len < min_len:
                    min_len = curr_len
                curr_sum -= l[left]
                left += 1
        right += 1
    return min_len if min_len != float('inf') else 0

#test_suite.run_test_suite(minimal_len_subarray_sum_05102026, test_cases, 7)


""" 

TODAY NO MASTERY CHALLENGE ILL SOLVE 1 PROBLEM THAT I ALREDY KNOW FOR EACH PATTERN I KNOW 05/10/2026

 """


def binary_search_05102026(l: list[int], t: int) -> int:
    left = 0
    right = len(l) - 1
    while left <= right:
        mid = left + (right - left) // 2
        if t == l[mid]:
            return l[mid]
        elif t > l[mid]:
            left = mid + 1
        else:
            right = mid - 1
    return -1 



def two_sum_05102026(l: list[int], t: int) -> tuple[int, int] | None:
    map = dict()
    for i, v in enumerate(l):
        c = t - v
        if c in map:
            return (i, map[c])
        map[v] = i
    return None

def min_max_interval(l: list[int]) -> list[int]:
    min_val_i = l.index(min(l))
    max_val_i = l.index(max(l))
    return l[min(min_val_i, max_val_i) + 1, max(min_val_i, max_val_i)]

def max_profit_05102026(l: list[int]) -> int:
    max_prf = 0
    cur_prf = 0
    min_prc = float('inf')
    for _, v in enumerate(l):
        if v < min_prc:
            min_prc = v
        cur_prf = v - min_prc
        if cur_prf > max_prf:
            max_prf = cur_prf
    return max_prf

def sum_subarray_size_k_05102026(l:list[int], k: int) -> int:
    max_sum = sum(l[:k])
    win_sum = max_sum
    for i in range(k, len(l)):
        win_sum += l[i] - l[i - k]
        if win_sum > max_sum:
            max_sum = win_sum
    return max_sum

def longest_substring_without_repeating_05102026(s: str) -> str:
    l = list(s)
    map = dict()
    left = 0
    right = 0
    max_len = 0
    current_len =0
    while right <= len(l) - 1:
        if l[right] in map:
            if map[right] >= left:
                left = map[right] + 1
            current_len = right - left + 1
            if current_len > max_len:
                max_len = current_len
        map[l[right]] = right
        right += 1
    return max_len


def min_subarray_greater_equal_target_05102026(l: list[int], t: int) -> int:
    left = 0
    right = 0 
    curr_sum = 0
    curr_len = 0
    min_len = float('inf')
    for right in range(len(l)):
        if right >= left:
            curr_sum += l[right]
            while curr_sum >= t:
                curr_len = right - left + 1
                if curr_len < min_len:
                    min_len = curr_len
                curr_sum -= l[left]
                left += 1
    return min_len if min_len != float('inf') else 0



def move_zeroes_05102026(l: list[int]) -> list[int]:
    write_ptr = 0
    read_ptr = 0 
    while read_ptr <= len(l) - 1:
        if l[read_ptr] != 0:
            if read_ptr >= write_ptr:
                l[read_ptr], l[write_ptr] = l[write_ptr], l[read_ptr]
                write_ptr += 1
                read_ptr += 1
        else:
            read_ptr += 1
                
                
