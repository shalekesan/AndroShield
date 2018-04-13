﻿using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Threading;
using System.Web;
using System.Web.Configuration;
using System.Web.UI;
using System.Web.UI.WebControls;
using Types;
using APKInfoExtraction;
using TaintAnalysis;


namespace AndroApp.Web_Forms
{
    public partial class apkUploadPage : System.Web.UI.Page
    {
        //string apkName;
        //string uploadedFileName="";
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                userEmail.Text = Session["username"].ToString();
            }
        }
        protected void signupNav_Click(object sender, EventArgs e)
        {
            Response.Redirect("userProfilePage.aspx");
        }
        protected void logoutButton_Click(object sender, EventArgs e)
        {
            Session["username"] = "";
            Session["userAccount"] = "";
            Response.Redirect("homePage.aspx");
        }

        protected void uploadBtn_Click(object sender, EventArgs e)
        {
            if (IsPostBack)
            {
                Session["fileOK"] = false;
                if (apkUpload.HasFile)
                {
                    Session["fileExtension"] =
                        System.IO.Path.GetExtension(apkUpload.FileName).ToLower();
                    String[] allowedExtensions =
                        {".apk"};
                    for (int i = 0; i < allowedExtensions.Length; i++)
                    {
                        if (Session["fileExtension"].ToString() == allowedExtensions[i])
                        {
                            Session["fileOK"] = true;
                        }
                    }
                }

                if ((bool)Session["fileOK"])
                {
                    try
                    {
                        apkUpload.PostedFile.SaveAs("C:\\GPTempDir\\"
                            + apkUpload.FileName);
                        analyzeBtn.Enabled = true;
                        Session["currentReportName"] = apkUpload.FileName;
                    }
                    catch (Exception ex)
                    {
                    }
                }
                else
                {
                    //uploadBtn.Text = "Cannot accept files of this type.";
                }
            }
        }

        protected void analyzeBtn_Click(object sender, EventArgs e)
        {
            Session["apkName"] = Session["currentReportName"].ToString();
            Session["apkPath"] = "C:\\GPTempDir\\" + Session["apkName"].ToString();

            analyzeApk(Session["apkPath"].ToString());

            Response.Redirect("reportAnalysisPage.aspx");
        }
        void analyzeApk(string path)
        {
            Session["user"] = userAccountTable.findUserByEmail(Session["username"].ToString());
            Session["apkInfoExtraction"] = new APKInfoExtractor(path);
            ((APKInfoExtractor)Session["apkInfoExtraction"]).startExtraction();
            Session["taintAnalysis"] = new TaintAnalyser(((APKInfoExtractor)Session["apkInfoExtraction"]).realApkPath);
            //TODO: apkRiskLevel determination
            Session["apk"] = apkInfoTable.insertAPKInfo(0, Session["apkName"].ToString(), ((APKInfoExtractor)Session["apkInfoExtraction"]).minSDKVersion, ((APKInfoExtractor)Session["apkInfoExtraction"]).targetSDKVersion, ((APKInfoExtractor)Session["apkInfoExtraction"]).packageName, ((APKInfoExtractor)Session["apkInfoExtraction"]).versionCode, ((APKInfoExtractor)Session["apkInfoExtraction"]).versionName, ((APKInfoExtractor)Session["apkInfoExtraction"]).testFlag, ((APKInfoExtractor)Session["apkInfoExtraction"]).debuggableFlag, ((APKInfoExtractor)Session["apkInfoExtraction"]).backupFlag, ((APKInfoExtractor)Session["apkInfoExtraction"]).supportedArchitectures.all, ((APKInfoExtractor)Session["apkInfoExtraction"]).supportedArchitectures.armeabi, ((APKInfoExtractor)Session["apkInfoExtraction"]).supportedArchitectures.armeabi_v7a, ((APKInfoExtractor)Session["apkInfoExtraction"]).supportedArchitectures.arm64_v8a, ((APKInfoExtractor)Session["apkInfoExtraction"]).supportedArchitectures.x86, ((APKInfoExtractor)Session["apkInfoExtraction"]).supportedArchitectures.x86_64, ((APKInfoExtractor)Session["apkInfoExtraction"]).supportedArchitectures.mips, ((APKInfoExtractor)Session["apkInfoExtraction"]).supportedArchitectures.mips64);
            Session["apkReport"] = reportTable.addNewReport(DateTime.Now.Date, true, false, ((apkInfoTable)Session["apk"]).apkInfoID, ((userAccountTable)Session["user"]).ID);
            reportTable test = (reportTable)Session["apkReport"];
            Session["apkVulnerabilities"] = new List<Vulnerability>();
            ((List<Vulnerability>)Session["apkVulnerabilities"]).AddRange(((APKInfoExtractor)Session["apkInfoExtraction"]).vulnerabilities);
            ((List<Vulnerability>)Session["apkVulnerabilities"]).AddRange(((TaintAnalyser)Session["taintAnalysis"]).vulnerabilities);

            //Session["dbVulnerability"] = new vulnerabilityTable();
            for(int i=0; i< ((List<Vulnerability>)Session["apkVulnerabilities"]).Count; i++)
            {
                Session["dbVulnerability"] = vulnerabilityTable.addOrFindVulnerability(((List<Vulnerability>)Session["apkVulnerabilities"])[i].severity, ((List<Vulnerability>)Session["apkVulnerabilities"])[i].category, ((List<Vulnerability>)Session["apkVulnerabilities"])[i].type);
                ((reportTable)Session["apkReport"]).createRelationBetweenReportAndVulnerability(((reportTable)Session["apkReport"]).reportId, ((vulnerabilityTable)Session["dbVulnerability"]).vulnID, ((List<Vulnerability>)Session["apkVulnerabilities"])[i].extraInfo);
            }

            Session["upladPagePermissions"] = ((APKInfoExtractor)Session["apkInfoExtraction"]).permissions;
            
            for(int i=0; i<((string[])Session["upladPagePermissions"]).Length; i++)
            {
                permissionTable.addNewPermission(((string[])Session["upladPagePermissions"])[i]);
                ((apkInfoTable)Session["apk"]).createRelationBetweenAPKInfoAndPermission(((apkInfoTable)Session["apk"]).apkInfoID, ((string[])Session["upladPagePermissions"])[i]);
            }
            Session["launchableActivities"] = ((APKInfoExtractor)Session["apkInfoExtraction"]).launchableActivities;
            for (int i=0; i<((string[])Session["launchableActivities"]).Length; i++)
            {
                launchableActivityTable.addNewActivity(((string[])Session["launchableActivities"])[i], ((apkInfoTable)Session["apk"]).apkInfoID);
            }

            Session["reportID"] = ((reportTable)Session["apkReport"]).reportId;
            //Session["apkInfo"] = apkInfoExtraction;
            //Session["taint"] = taintAnalysis;
        }

    }

}