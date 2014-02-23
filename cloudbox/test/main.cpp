#include <gmock/gmock.h>
#include <conio.h>


std::string kStoragePath;

GTEST_API_ int main(int argc, char** argv) {

    if (argc < 2) {
        printf("Usage: %s [storage_path] [hold_after_test_completed] [gtest options ...]", argv[0]);
        return -1;
    }

    kStoragePath = argv[1];

    testing::InitGoogleMock(&argc, argv);
    const int retVal = RUN_ALL_TESTS();

    if (argc == 3 && strcmp(argv[2], "hold_after_test_completed") == 0) {
        printf("\nPress any key to complete.");
        getch();
    }
    return retVal;
}