#include "XmlParser.h"
#include <fstream>;

XmlParser::XmlParser(string relativePath)
{
	xmlContent = getFileLines(relativePath);
	doc.parse<0>(&xmlContent[0]);  // 0 means set parse flags to their default values
}

string XmlParser::getFileLines(string relativePath)
{
	//Read file content to stringstream
	ifstream file(relativePath);

	stringstream buffer;
	buffer << file.rdbuf(); //rdbuf() returns a pointer to the filebuf

	file.close();

	return buffer.str();
}
bool XmlParser::BackupModeEnabled()
{
	xml_node<> *applicationElement = doc.first_node()->first_node("application", 0, false);
	if (applicationElement->first_attribute("android:allowBackup") != NULL)
	{
		xml_attribute<> *allowBackupAttr = applicationElement->first_attribute("android:allowBackup", 0, false);
		string attrValue = allowBackupAttr->value();
		if (allowBackupAttr == NULL || attrValue == "false")
			return false;
		else return true;
	}
	return false;
}
bool XmlParser::DebugModeEnabled()
{
	xml_node<>* node = doc.first_node()->first_node("application", 0, false);
	if (node->first_attribute("android:debuggable") != NULL)
	{
		xml_attribute<> *debuggableAttr = node->first_attribute("android:debuggable", 0, false);
		string attrVal = debuggableAttr->value();
		if (debuggableAttr == NULL || attrVal == "false")
			return false;
		else return true;
	}
	return false;
}
void XmlParser::getAppPermissionsExplicitProtectionLevels()
{
	string permissionName;
	string permissionProtectionLevel;
	xml_node<>* manifestNode = doc.first_node("manifest", 0, false);

	//Loop to find permissions with explicit protection levels
	for (xml_node<> *child = manifestNode->first_node("permission", 0, false); child; child = child->next_sibling("permission", 0, false))
	{
		permissionProtectionLevel = (*(*child).first_attribute("android:protectionLevel", 0, false)).value();
		permissionName = (*(*child).first_attribute("android:name", 0, false)).value();

		if (!permissionProtectionLevel.empty())
		{
			permissionsWithProtectionLevels[permissionName] = permissionProtectionLevel;
			appPermissionsWithProtectionLevels.push_back(make_pair(permissionName, permissionProtectionLevel));
		}
	}
}
void XmlParser::getAppPermissionsWithoutProtectionLevels()
{
	string permissionName;
	xml_node<>* manifestNode = doc.first_node("manifest", 0, false);

	for (xml_node<> *child = manifestNode->first_node("uses-permission", 0, false); child; child = child->next_sibling("uses-permission", 0, false))
	{
		permissionName = (*(*child).first_attribute("android:name", 0, false)).value();

		if (permissionsWithProtectionLevels[permissionName].empty())
			appPermissionsWithoutProtectionLevels.push_back(permissionName);
	}
}
void XmlParser::getAppPermissionsForSDK23OrHigher()
{
	string permissionName;
	xml_node<>* manifestNode = doc.first_node("manifest", 0, false);
	for (xml_node<> *child = manifestNode->first_node("uses-permission-sdk-23", 0, false); child; child = child->next_sibling("uses-permission-sdk-23", 0, false))
	{
		permissionName = (*(*child).first_attribute("android:name", 0, false)).value();

		if (permissionsWithProtectionLevels[permissionName].empty())
			appPermissionsForSDK23orHigher.push_back(permissionName);
	}
}
string XmlParser::getComponentPermissionString(xml_node<>* child)
{
	string componentName = (*(*child).first_attribute("android:name", 0, false)).value();
	if ((*child).first_attribute("android:permission", 0, false))
	{
		string permissionName = (*(*child).first_attribute("android:permission", 0, false)).value();
		return (permissionName);
	}
	else return "";
}
void XmlParser::getActivitiesPermissions()
{
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	string compPermissionString;
	string componentName;
	for (xml_node<> *child = applicationNode->first_node("activity", 0, false); child; child = child->next_sibling("activity", 0, false))
	{
		componentName = (*(*child).first_attribute("android:name", 0, false)).value();
		compPermissionString = getComponentPermissionString(child);
		if (compPermissionString != "")
		{
			activitiesPermissions.push_back(make_pair(componentName, compPermissionString));
		}
		else {
			activitiesPermissions.push_back(make_pair(componentName, ""));
		}
	}
}
void XmlParser::getServicesPermissions()
{
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	string compPermissionString;
	string componentName;
	for (xml_node<> *child = applicationNode->first_node("service", 0, false); child; child = child->next_sibling("service", 0, false))
	{
		componentName = (*(*child).first_attribute("android:name", 0, false)).value();
		compPermissionString = getComponentPermissionString(child);
		if (compPermissionString != "")
		{
			servicesPermissions.push_back(make_pair(componentName, compPermissionString));
		}
		else {
			servicesPermissions.push_back(make_pair(componentName, ""));
		}
	}
}
void XmlParser::getProvidersPermissions()
{
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	string compPermissionString;
	string componentName;
	for (xml_node<> *child = applicationNode->first_node("provider", 0, false); child; child = child->next_sibling("provider", 0, false))
	{
		componentName = (*(*child).first_attribute("android:name", 0, false)).value();
		compPermissionString = getComponentPermissionString(child);
		if (compPermissionString != "")
		{
			contentProvidersPermissions.push_back(make_pair(componentName, compPermissionString));
		}
		else {
			contentProvidersPermissions.push_back(make_pair(componentName, ""));
		}
	}
}
void XmlParser::getReceiversPermissions()
{
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	string compPermissionString;
	string componentName;
	for (xml_node<> *child = applicationNode->first_node("receiver", 0, false); child; child = child->next_sibling("receiver", 0, false))
	{
		componentName = (*(*child).first_attribute("android:name", 0, false)).value();
		compPermissionString = getComponentPermissionString(child);
		if (compPermissionString != "")
		{
			broadcastReceiversPermissions.push_back(make_pair(componentName, compPermissionString));
		}
		else {
			broadcastReceiversPermissions.push_back(make_pair(componentName, ""));
		}
	}
}
string XmlParser::isExported(xml_node<>* child)
{
	string exportedAttrValue = "";
	string componentName = "";

	if (child->first_attribute("android:exported", 0, false))
		exportedAttrValue = child->first_attribute("android:exported", 0, false)->value();

	if (exportedAttrValue != "false" && (child->first_node("intent-filter", 0, false) || exportedAttrValue == "true"))
		componentName = child->first_attribute("android:name", 0, false)->value();
	return componentName;
}
void XmlParser::getActivities()
{
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	string componentName;

	for (xml_node<> *child = applicationNode->first_node("activity", 0, false); child; child = child->next_sibling("activity", 0, false))
	{
		componentName = (*(*child).first_attribute("android:name", 0, false)).value();
		activities.push_back(componentName);
	}
}
void XmlParser::getServices()
{
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	string componentName;
	for (xml_node<> *child = applicationNode->first_node("service", 0, false); child; child = child->next_sibling("service", 0, false))
	{
		componentName = (*(*child).first_attribute("android:name", 0, false)).value();
		services.push_back(componentName);
	}
}
void XmlParser::getContentProviders()
{
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	string componentName;
	for (xml_node<> *child = applicationNode->first_node("provider", 0, false); child; child = child->next_sibling("provider", 0, false))
	{
		componentName = (*(*child).first_attribute("android:name", 0, false)).value();
		contentProviders.push_back(componentName);
	}
}
void XmlParser::getBroadcastReceivers()
{
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	string componentName;
	for (xml_node<> *child = applicationNode->first_node("receiver", 0, false); child; child = child->next_sibling("receiver", 0, false))
	{
		componentName = (*(*child).first_attribute("android:name", 0, false)).value();
		broadcastReceivers.push_back(componentName);
	}
}
void XmlParser::getExportedActivities()
{
	string componentName;
	xml_node<>* sdkNode = doc.first_node("manifest", 0, false)->first_node("uses-sdk", 0, false);
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	for (xml_node<> *child = applicationNode->first_node("activity", 0, false); child; child = child->next_sibling("activity", 0, false))
	{
		componentName = isExported(child);
		if (componentName == "")
			continue;
		else
			exportedActivities.push_back(componentName);
	}
}
void XmlParser::getExportedServices()
{
	string componentName;
	xml_node<>* sdkNode = doc.first_node("manifest", 0, false)->first_node("uses-sdk", 0, false);
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	for (xml_node<> *child = applicationNode->first_node("service", 0, false); child; child = child->next_sibling("service", 0, false))
	{
		componentName = isExported(child);
		if (componentName == "")
			continue;
		else
			exportedServices.push_back(componentName);
	}
}
void XmlParser::getExportedContentProviders()
{
	string componentName;
	bool exportAttributeDefaultValueIsTrue = false;
	xml_node<>* sdkNode = doc.first_node("manifest", 0, false)->first_node("uses-sdk", 0, false);
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);

	char* tarSDK = sdkNode->first_attribute("android:targetSdkVersion", 0, false)->value();

	if (tarSDK && atoi(tarSDK) <= 16)
		exportAttributeDefaultValueIsTrue = true;

	for (xml_node<> *child = applicationNode->first_node("provider", 0, false); child; child = child->next_sibling("provider", 0, false))
	{
		if ((child->first_attribute("android:exported", 0, false) && child->first_attribute("android:exported", 0, false)->value() == "true") ||
			(!child->first_attribute("android:exported", 0, false) && exportAttributeDefaultValueIsTrue))
		{
			componentName = child->first_attribute("android:name", 0, false)->value();
			exportedContentProviders.push_back(componentName);
		}
	}
}
void XmlParser::getExportedBroadcastReceivers()
{
	string exportedComponentsstring = "Services:\n";
	string componentName;
	bool exportAttributeDefaultValueIsTrue = false;
	xml_node<>* sdkNode = doc.first_node("manifest", 0, false)->first_node("uses-sdk", 0, false);
	xml_node<>* applicationNode = doc.first_node("manifest", 0, false)->first_node("application", 0, false);
	for (xml_node<> *child = applicationNode->first_node("receiver", 0, false); child; child = child->next_sibling("receiver", 0, false))
	{
		componentName = isExported(child);
		if (componentName == "")
			continue;
		else
			exportedComponentsstring += componentName + '\n';
	}
}
bool XmlParser::ExternalStorage()
{
	xml_node<>* manifestNode = doc.first_node("manifest", 0, false);
	string permissionName = "";
	for (xml_node<> *child = manifestNode->first_node("uses-permission", 0, false); child; child = child->next_sibling("uses-permission", 0, false))
	{
		permissionName = (*(*child).first_attribute("android:name", 0, false)).value();
		if (permissionName == "android.permission.WRITE_EXTERNAL_STORAGE")
			return true;
	}
	return false;
}
XmlParser::~XmlParser()
{
	xml_node<>* manifestNode = doc.first_node("manifest", 0, false);
}