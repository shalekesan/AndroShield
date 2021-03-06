#include "HttpRequestsDetector.h"
using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;

namespace DynamicAnalysis
{
	HttpRequestsDetector::HttpRequestsDetector(String^ filePath)
	{
		detectHttp(filePath);
	}
	void HttpRequestsDetector::detectHttp(String^ filePath)
	{
		List<String^>^ httpRequests = gcnew List<String^>();

		FileStream^ FS = gcnew FileStream(filePath, FileMode::Open);
		StreamReader^ SR = gcnew StreamReader(FS);
		String^ line;
		bool addToRequests = true;

		while (SR->Peek() != -1)
		{
			line = SR->ReadLine();
			if (line->Contains("sendto") && line->Contains("HTTP"))
			{
				array<String^>^ httpRequest = line->Split(gcnew array<String^>{ "\\r\\n" }, StringSplitOptions::RemoveEmptyEntries);

				for (int i = 0; i < httpRequest->Length; i++)
				{
					if (httpRequest[i]->Contains("Host: "))
					{
						httpRequest = httpRequest[i]->Split(gcnew array<String^>{ ": " }, StringSplitOptions::RemoveEmptyEntries);
						if (httpRequest->Length == 2)
						{
							addToRequests = true;
							for (int i = 0; i < httpRequests->Count; i++)
							{
								if (httpRequest[1] == httpRequests[i])
									addToRequests = false;
							}

							if (addToRequests)
								httpRequests->Add(httpRequest[1]);
						}
						break;
					}
				}
			}
		}
		SR->Close();
		inSecureVulnerability.extraInfo = "";
		if (httpRequests->Count != 0) {
			inSecureVulnerability.type = "dynamic";
			inSecureVulnerability.category = "Http Requests";
			inSecureVulnerability.severity = 0.7;
			for (int i = 0; i < httpRequests->Count; i++)
			{
				inSecureVulnerability.extraInfo += httpRequests[i];
				if (i != httpRequests->Count - 1)
					inSecureVulnerability.extraInfo += ", ";
			}
		}
	}
}


