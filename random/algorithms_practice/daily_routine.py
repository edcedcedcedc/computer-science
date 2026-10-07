import test_suite

""" 
WARMP UP sliding window // min len subarray sum >= target 

l 
left pointer = 0
right pointer = 0
min len = float inf 
cur len = 0
cur sum = 0

main loop
    if right pointer >= left pointer 
        cur sum += l(right pointer) 
        inner loop cur sum >= t
            cur len = right - left + 1
            if cur len < min len
                min len = cur len
            cur sum -= l(left pointer)
            left pointer += 1 
    right pointer += 1   
 """

def min_len_subarray_sum_06102026(l, t):
    left_p = 0
    cur_len = 0
    min_len = float('inf')
    cur_sum = 0 
    for right_p in range(len(l)):
        if right_p >= left_p:
            cur_sum += l[right_p]
            while cur_sum >= t:
                cur_len = right_p - left_p + 1
                if cur_len < min_len:
                    min_len = cur_len
                cur_sum -= l[left_p]
                left_p += 1
    return min_len if min_len != float('inf') else 0


                
""" 
REVIEW hash map
 """

def duplicate2_06102026(l,k):
    map = dict()
    for i in range(len(l)):
        if l[i] in map:
            d = abs(map[l[i]] - i)
            if d <= k:
                return True 
        map[l[i]] = i 


""" MASTERY CHALLENGE  prefix sum"""       


class RangeSumQuery:
    def __init__(self, nums):
        self.prefix = list()
        self.prefix.append(nums[0])
        for i in range(1, len(nums)):
            self.prefix.append(self.prefix[i - 1] + nums[i])
    def sum_range(self, i, j):
        return self.prefix[j] - self.prefix[i - 1]
        







""" 
WARM UP 10/07/2026
"""

class PrefixSum():
    def __init__(self, arr):
        self.arr = arr
        self.prefix_arr = list()
        self.prefix_arr.append(arr[0])
        for i in range(1, len(arr)):
            self.prefix_arr.append(self.prefix_arr[i - 1] + self.arr[i])

   
    def sum_interval(self,i: int, j: int) -> int:
        if i == 0:
            return self.prefix[j]
        return self.prefix_arr[j] - self.prefix_arr[i - 1]




""" 

REVIEW // sliding window 
"""

def longest_substring_without_repeating_07_10_2026(s: str) -> int:
    array = list(s)
    hash_map = dict()
    pointer_left = 0
    max_len = 0
    current_len = 0
    for pointer_right in range(len(array)):
        if array[pointer_right] in hash_map:
            if hash_map[array[pointer_right]] >= pointer_left:
                pointer_left = hash_map[array[pointer_right]] + 1
                current_len = pointer_right - pointer_left + 1
        if current_len > max_len: #this should be hear for all char unique case 
            max_len = current_len     
        hash_map[array[pointer_right]] = pointer_right

    return max_len

