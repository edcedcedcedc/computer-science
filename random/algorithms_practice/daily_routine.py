

#02/10/2026 morning 

#hash map O(n)
def two_sum(array: int, t: int):
    map = dict()
    for i, v in enumerate(array):
        c = t - v
        if c in map:
            return (i,map[c])
        else:
            map[v] = i
    return None    



#sliding window O(n)
def max_sum(array: int, k: int):
    window_sum = sum(array[:k])
    max_sum = window_sum
    for i in range(k, len(array)):
        window_sum += array[i] - array[i - k]
        if window_sum > max_sum:
            max_sum = window_sum
    return max_sum 


#two pointers O(n) move zeros to the right, array mutation // slightly different problem on two pointers to test pattern


def zero_to_the_right(a: int):
    write_ptr = 0
    read_ptr = 0
    for i in range(len(a)):
        if a[i] != 0:
            if read_ptr > write_ptr:
                a[write_ptr], a[read_ptr] = a[read_ptr], a[write_ptr]
            read_ptr += 1
            write_ptr += 1
        else:
            read_ptr += 1  
    return a 

def zero_to_the_right_with_temp(a: int):
    write_ptr = 0
    temp = 0 
    for read_ptr in range(len(a)):
        if a[read_ptr] != 0:
            if read_ptr > write_ptr:
                temp = a[write_ptr]
                a[write_ptr]= a[read_ptr]
                a[read_ptr] = temp
            write_ptr += 1 
    return a 
print(zero_to_the_right([1,1,1,1,0]))   

# two pointers, O(n) move zeros to the right, with temp, read pointer is actually i



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


#02/10/2026 evening 
#hash map, greedy running state, two pointers, binary search; I did this from mind, no prep, recall
def two_sum(a: int, t: int):
    map = dict()
    for i,v in range(len(a)): #mistake enumarate
        c = t - v
        if c in map:
            return (i, map[c])
        map[v] = i
    return None


def binary_search(a: int, t: int):
    left = 0
    right = len(a) - 1
    while left <= right:
        mid = left + (right - left) // 2
        if t == a[mid]:
            return a[mid]
        elif t > a[mid]:
            left = mid + 1
        else:
            right = mid - 1
    return -1


def palidrome(a: str):
    left = 0
    right = len(a) - 1
    while left < right:
        while left < right and not a[left].isalpha():
            left += 1
        while left < right and not a[right].isalpha():
            right -= 1
        if a[left] != a[right]:
            return False 
        left += 1
        right -= 1
    return True


def best_time_buy_sell(a:int):
    max_profit = 0
    curr_profit = 0
    min_price = float('inf')
    for i in range(len(a)):
        if a[i] < min_price:
            min_price = a[i]
        curr_profit = a[i] - min_price
        if curr_profit > max_profit:
            max_profit = curr_profit 
    return max_profit  


#02/10/2026 11:46PM
def two_sum(nums: int, t: int):
    map = dict()
    for i, v in enumerate(nums):
        c = t - v
        if c in map:
            return (i, map[c])
        map[v] = i
    return None


def valid_palidrome(s: str):
    l = list(s)
    left = 0
    right = len(l) - 1
    while left < right:
        while left < right and not l[left].isalpha():
            left += 1    
        while left < right and not l[right].isalpha():
            right -= 1
        if l[left] != l[right]:
            return False 
        left += 1
        right -= 1
    return True 


def best_time_sell_buy1(a: list[int]):
    max_profit = 0
    curr_profit = 0
    min_price = float('inf')
    for i in range(len(a)):
        if a[i] < min_price:
            min_price = a[i]
        curr_profit = a[i] - min_price
        if curr_profit > max_profit:
            max_profit = curr_profit
    return max_profit 

def max_sum_subarray_size_k(a: list[int], k: int):
    max_sum = sum(a[:k])
    window_sum = max_sum
    for i in range(k,len(a)):
        window_sum += a[i] - a[i - k]
        if window_sum > max_sum:
            max_sum = window_sum
    return max_sum 
    
    
def print_interval_min_max(a: list[int]):
    index_min = a.index(min(a))
    index_max = a.index(max(a))
    a = a[index_min + 1: index_max]
    return a 


def binary_search(a: list[int], t: int):
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




# 03/10/2026

#warm any random problem from day before from scratch 
def reverse_string11(s: str):
    l = list(s)
    left = 0
    right = len(l) - 1
    while left < right:
        l[left], l[right] = l[right], l[left]
        right -= 1
        left += 1
    return "".join(l)


# 04/10/2026 night 


def binary_search041026night(l: list[int], t: int) -> int:
    left = 0
    right = len(l) - 1
    while left <= right:
        mid = left + (right - left) // 2
        if t == l[mid]:
            return mid
        elif t > l[mid]:
            left = mid + 1
        else:
            right = mid - 1
    return -1 


def move_zeros_041026night(l: list[int]) -> list[int]:
    write_ptr = 0
    read_ptr = 0
    while read_ptr < len(l):
        if l[read_ptr] != 0:
            if read_ptr > write_ptr:
                l[read_ptr], l[write_ptr] = l[write_ptr],l[read_ptr]
            read_ptr += 1
            write_ptr += 1
        else:
            read_ptr += 1    


def two_sum_sorted_041026night(l: list[int], t:int) -> tuple[int, int]:
    left = 0 
    right = len(l) - 1
    while left < right:
        if l[left] + l[right] < t:
            left += 1
        elif l[left] + l[right] > t:
            right -= 1
        else:
            return (left, right)

def two_sum_041026night(l: list[int], t: int) -> tuple[int, int]:
    map = dict()
    for i, v in enumerate(l):
        c = t - v
        if c in map:
            return (i, map[c])
        map[v] = i
    return None

def min_max_interval_041026night(l: list[int]) -> list[int]:
    min_index = l.index(min(l))
    max_index = l.index(max(l))
    return l[min_index + 1: max_index]


def best_time_buy_sell_041026night(l: list[int]) -> int: 
    max_profit = 0
    current_profit = 0
    min_price = float('inf')
    for i in range(len(l)):
        if l[i] < min_price:
            min_price = l[i]
        current_profit = l[i] - min_price 
        if current_profit > max_profit:
            max_profit = current_profit
    return max_profit 

def max_subarray_sum_size_k_041026night(l: list[int], k: int) -> int:
    max_sum = sum(l[:k])
    window_sum = max_sum 
    for i in range(k, len(l)):
        window_sum += l[i] - l[i - k]
        if window_sum > max_sum:
            max_sum = window_sum
    return max_sum 

def find_middle_value_two_pointers_041026night(l: list[int]) -> int:
    slow_ptr = 0
    fast_ptr = 0 
    while True:  
        if fast_ptr == len(l) - 1:
            return l[slow_ptr] 
        elif fast_ptr + 1 == len(l) - 1:
            return l[slow_ptr + 1]
        slow_ptr += 1
        fast_ptr += 2

        