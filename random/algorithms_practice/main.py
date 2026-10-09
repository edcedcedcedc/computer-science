
# two sum // hash map
# O(n^2)
# print(two_sum([2,11,15],9))
def two_sum(array, target):
    for i in range(len(array)):
        for j in range(len(array)):
            if array[j]+array[i] == target:
                return (i,j)
    return None



# hash map
# O(n)
def duplicate2_06102026(l,k):
    map = dict()
    for i in range(len(l)):
        if l[i] in map:
            d = abs(map[l[i]] - i)
            if d <= k:
                return True 
        map[l[i]] = i 


#print(two_sum_faster([2,7,11,15],9))
# two sum 
# O(n)
def two_sum_faster(array, target):
    seen = dict()
    for index, value in enumerate(array):
        complement = target - value
        if complement in seen:
            return (index, seen[complement])
        seen[value] = index
    return None


# two sum array sorted, // two pointers 
# time complexity O(n) space complexity O(1)
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


# palindrome // two pointers 
# O(n)
#print(palindrome("A man, a plan, a canal: Panama"))
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


#greedy running state// tracking dynamic state in a single pass // sliding window
#O(n)
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



#O(n) hash map, well could be done with set as well or any other way, this are easy on purpose 
def seen_twice(array):
    seen = dict()
    for i,v in enumerate(array):
        if v in seen:
            return True 
        seen[v] = i
    return False


#O(n) two pointers 
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

#O(n) greedy running state, dynamic state
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



#O(log n) binary search, with L + (R - L)/2
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



#O(n) linear search
def min_max_interval(l: list[int]) -> list[int]:
    min_val_i = l.index(min(l))
    max_val_i = l.index(max(l))
    return l[min(min_val_i, max_val_i) + 1, max(min_val_i, max_val_i)]

#O(n) two pointers fast and slow 
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

#O(n) variable size sliding window / hash map
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


#O(n) sliding window // minimal len subarray whose sum is greater or equal to target
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




#prefix sum O(n)
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

#monotonic stack O(n)
def next_great_element_08102026(l: list[int]) -> list[int]:
    output = -1 * len(l)
    stack = list()
    for i in range(len(l)):
        while len(stack) > 0 and l[i] > l[stack[-1]]:
            popped_i = stack.pop()
            output[popped_i] = l[i]
        stack.append(i)



#prefix sum / hash map // number of times the any len subarray equals key
""" 
I think I got the abstraction how I think on it, 

the previous basic hash map pattern(two sum problem) with complement got me thinking algebraically 
and programmatically the same time,

you assign the value but at the same time you approve the algebra,
so when we say prefix[j] - prefix[i - 1] = k and prefix[i - 1] = prefix[j] - k 

then in program when I do, if prefix_l[j] - k in map and 
after that map[prefix_l[j]] = map.get(prefix_l[j], 0) + 1 

I literally state the math behind it that prefix[i-1] = prefix[j] - k 
and this thinking reminded me the hash map basic pattern 
and the mathematical part of it that are used at the same time,
this eliminates the O(n^2) complexity to O(n) complexity

"""

def times_subarray_sum_equal_k(l: list[int], k: int) -> int:
    map = dict()
    map[0] = 1
    total_counts = 0
    prefix_l = list()
    prefix_l.append[l[0]]
    
    for i in range(1, len(l)):
        prefix_l.append(prefix_l[i - 1] + l[i])

    for j in range(len(l)): 
        if prefix_l[j] - k in map:
            total_counts += map[prefix_l[j] - k]
        map[prefix_l[j]] = map.get(prefix_l[j], 0) + 1
    
    return total_counts   
            



#recursion O(n^2) string concatenation
def reverse_string_(s:str) -> str:
    l = list(s) 
    def helper(i: int):
        if i == len(l) - 1:
            return l[i]
        else:
            return helper(i + 1) + l[i]
    print(helper(0))

#recursion O(n) list mutation 
def reverse_string__(s:str) -> str:
    l = list(s)
    acc = list()                                  
    def helper(i: int):
        if i == len(l):
            return 
        helper(i + 1)
        acc.append(l[i])
    helper(0)
    return "".join(acc)
