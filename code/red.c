if AvgLen <= MinThreshold 
    queue the packet 
if MinThreshold < AvgLen < MaxThreshold 
    calculate probability P 
    drop the arriving packet with probability P 
if MaxThreshold <= AvgLen 
    drop the arriving packet 
