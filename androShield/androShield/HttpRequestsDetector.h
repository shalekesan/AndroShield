#pragma once
using namespace System::Collections::Generic;
#include"Vulnerability.h"
using namespace Types;
namespace DynamicAnalysis
{
	public ref class HttpRequestsDetector
	{
	public:
		HttpRequestsDetector(String^);
		void detectHttp(String^);
		Vulnerability v;
	};
}