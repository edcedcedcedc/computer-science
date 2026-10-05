def two_sum(array, target):
    for i in range(len(array)):
        for j in range(len(array)):
            if array[j]+array[i] == target:
                return (i,j)
    return None
# two sum // hash map
# O(n^2)
# print(two_sum([2,11,15],9))

def two_sum_faster(array, target):
    seen = dict()
    for index, value in enumerate(array):
        complement = target - value
        if complement in seen:
            return (index, seen[complement])
        seen[value] = index
    return None
#print(two_sum_faster([2,7,11,15],9))
# two sum 
# O(n)


def two_sum_sorted_array(a: list[int], t: int) -> tuple[int, int]:
    left = 0
    right = len(a) - 1
    while left < right:
        curr_t = a[left] + a[right]
        if curr_t == t:
            return (left + 1, right +1)
        elif curr_t < t: 
            left += 1
        else:
            right -= 1
    return None 
# two sum array sorted, // two pointers 
# time complexity O(n) space complexity O(1)


def palindrome(text):
    left = 0
    right = len(text) - 1
    while left < right:
        while left < right and not text[left].isalpha():
            left += 1
        while left < right and not text[right].isalpha():
            right -= 1
        if text[right].lower() != text[left].lower():
            return False
        left += 1
        right -= 1
    return True
# palindrome // two pointers 
# O(n)
#print(palindrome("A man, a plan, a canal: Panama"))


def max_profit(sequence):
    max_profit = 0
    min_price = float('inf')
    for i in range(len(sequence)):
        if sequence[i] < min_price:
            min_price = sequence[i]
        else:
            if sequence[i] - min_price > max_profit:
                max_profit = sequence[i] - min_price
    return max_profit
#greedy running state// tracking dynamic state in a single pass // sliding window
#O(n)




def seen_twice(array):
    seen = dict()
    for i,v in enumerate(array):
        if v in seen:
            return True 
        seen[v] = i
    return False
#O(n) hash map, well could be done with set as well or any other way, this are easy on purpose 


def reverse_string(array: str):
    left = 0
    right = len(array) - 1
    temp = ""
    while left < right:
        temp = array[left]
        array[left] = array[right] 
        array[right] = temp
        left += 1
        right -= 1
        temp = ""
    return array 
#O(n) two pointers 

def max_consecutive(array: int):
    max_cons = 0
    curr_cons = 0
    for _, value in enumerate(array):
        if value == 1:
            curr_cons += 1
            if curr_cons > max_cons:
                max_cons = curr_cons
        else:
            curr_cons = 0
    return max_cons
#O(n) greedy running state, dynamic state



def search_target(a: int, t:int):
    left = 0
    right = len(a) - 1
    while left <= right:
        mid = left + (right - left) // 2
        if t == a[mid]:
            return mid
        elif t > a[mid]:
            left = mid + 1
        else:
            right = mid - 1
    return -1
#O(log n) binary search, with L + (R - L)/2



def min_max_interval(l: list[int]) -> list[int]:
    min_val_i = l.index(min(l))
    max_val_i = l.index(max(l))
    return l[min(min_val_i, max_val_i) + 1, max(min_val_i, max_val_i)]
#O(n) linear search

def find_middle_value_using_two_pointers(l: list[int]):
    slow_ptr = 0
    fast_ptr = 0
    while True:
        if fast_ptr == len(l) - 1:
            return l[slow_ptr]
        elif fast_ptr + 1 == len(l) - 1:
            return l[slow_ptr + 1]
        slow_ptr += 1
        fast_ptr += 2
#O(n) two pointers fast and slow 

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
#O(n) variable size sliding window / hash map


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
#O(n) sliding window // minimal len subarray whose sum is greater or equal to target