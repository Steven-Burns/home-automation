template<typename T> 
void 
Sampler<T>::TakeSample(ulong clock, T newValue)
{
    // swap the current and previous ptrs.  Result is that everytime this method is called, *current is the most recent
    // sample taken and *previous is the prior one
    Sample<T>* prev = previous;
    previous = current;
    current = prev;

    current->clock = clock;
    current->value = newValue;
}



template<typename T> 
bool 
Sampler<T>::HasSampleChanged() const
{
    bool result = current->value != previous->value;

    // check to see that the timestamps have increased when there is a delta. If not, then why is the pin changed?
    // if (result) 
    // {
    //   Serial.print(current->clock); Serial.print(' '); Serial.println(previous->clock);
    // }
    return result;
}



// const char *
// PinSampler::ToString() const
// {
//     snprintf(
//         (char *)toStringBuf,
//         toStringBufChars,
//         "c %1i %04X | p %1i %04X",
//         current->pinVal, current->clock, previous->pinVal, previous->clock);
//     return toStringBuf;
// }