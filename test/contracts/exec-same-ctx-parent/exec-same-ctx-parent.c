#include "../elrond/context.h"
#include "../elrond/bigInt.h"

byte parentKeyA[] =  "parentKeyA......................";
byte parentDataA[] = "parentDataA";
byte parentKeyB[] =  "parentKeyB......................";
byte parentDataB[] = "parentDataB";
byte parentFinishA[] = "parentFinishA";
byte parentFinishB[] = "parentFinishB";

byte parentTransferReceiver[] = "parentTransferReceiver..........";
byte parentTransferValue[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42};
byte parentTransferData[] = "parentTransferData";

byte executeValue[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,99};
u32 executeArgumentsLengths[] = {32, 6};
byte executeArgumentsData[] = "childTransferReceiver...........qwerty";

void finishResult(int);
u32 reverseU32(u32);

void parentFunctionPrepare() {
	storageStore(parentKeyA, parentDataA, 11);
	storageStore(parentKeyB, parentDataB, 11);
	finish(parentFinishA, 13);
	finish(parentFinishB, 13);
	int result = transferValue(
			parentTransferReceiver,
			parentTransferValue,
			parentTransferData,
			18
	);
	finishResult(result);
}

void parentFunctionWrongCall() {
	parentFunctionPrepare();
	byte childAddress[] = "wrongSC.........................";
	byte functionName[] = "childFunction";

	int result = executeOnSameContext(
			50000,
			childAddress,
			executeValue,
			functionName,
			13,
			2,
			(byte*)executeArgumentsLengths,
			executeArgumentsData
	);
	finishResult(result);
}

void parentFunctionChildCall() {
	parentFunctionPrepare();
	byte childAddress[] = "childSC.........................";
	byte functionName[] = "childFunction";
	int result = executeOnSameContext(
			200000,
			childAddress,
			executeValue,
			functionName,
			13,
			2,
			(byte*)executeArgumentsLengths,
			executeArgumentsData
	);

	// TODO assert that the storage changes made by the child are visible here
	finishResult(result);
}

void parentFunctionChildCall_BigInts() {
	bigInt intA = bigIntNew(84); 
	bigInt intB = bigIntNew(96);
	bigInt intC = bigIntNew(1024);

	byte argumentSize = sizeof(bigInt);

	// All SmartContracts expect their integer arguments in Big Endian form, so
	// we need to reverse them (we're in Little Endian here) in order to pass
	// them to the childSC.
	bigInt arguments[] = {
		reverseU32(intA),
		reverseU32(intB),
		reverseU32(intC)
	};
	int argumentLengths[3] = {argumentSize, argumentSize, argumentSize};

	byte childAddress[] = "childSC.........................";
	byte functionName[] = "childFunction_BigInts";
	int result = executeOnSameContext(
			200000,
			childAddress,
			executeValue,
			functionName,
			21,
			3,
			(byte*)argumentLengths,
			(byte*)arguments
	);
	finishResult(result);
}

u32 reverseU32(u32 value) {
	u32 lastByteMask = 0x00000000000000FF;
	u32 result = 0;
	int size = sizeof(value);
	for (int i = 0; i < size; i++) {
		byte lastByte = value & lastByteMask;
		value >>= 8;

		result <<= 8;
		result += lastByte;
	}
	return result;
}

void finishResult(int result) {
	if (result == 0) {
		byte message[] = "succ";
		finish(message, 4);
	}
	if (result == 1) {
		byte message[] = "fail";
		finish(message, 4);
	}
	if (result != 0 && result != 1) {
		byte message[] = "unkn";
		finish(message, 4);
	}
}
