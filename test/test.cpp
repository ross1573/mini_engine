import mini;

void AllocatorTest();
void ArrayTest();
void QueueTest();
void SharedPtrTest();

void TestMain()
{
    mini::PrintLine("\n");
    {
        AllocatorTest(); mini::PrintLine("Allocator Test Passed");
        ArrayTest(); mini::PrintLine("Array Test Passed");
        QueueTest(); mini::PrintLine("Queue Test Passed");
        SharedPtrTest(); mini::PrintLine("SharedPtr Test Passed");
    }
    mini::PrintLine("\n");
}