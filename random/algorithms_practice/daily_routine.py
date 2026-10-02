
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