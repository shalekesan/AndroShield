﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace AndroApp.Web_Forms
{
    public partial class user_sReportsPage : System.Web.UI.Page
    {
        List<KeyValuePair<int, string>> userReports;
        List<string> apkNames;
        protected void Page_Load(object sender, EventArgs e)
        {
            string currentUserEmail = Session["username"].ToString();
            userEmail.Text = Session["username"].ToString();
            userReports= userAccountTable.getReportsOfThisUser(currentUserEmail);
            apkNames = new List<string>();

            HyperLink link = new HyperLink();
            link.NavigateUrl = "~/Web Forms/reportAnalysisPage.aspx";
            for (int i=0; i<userReports.Count; i++)
            {
                TableRow row = new TableRow();
                String[] info = userReports[i].Value.Split('#');
                TableCell apkName = new TableCell();
                apkName.Controls.Add(link);
                apkName.Text = info[0];
                apkNames.Add(info[0]);

                TableCell packageName = new TableCell();
                packageName.Controls.Add(link);
                packageName.Text = info[1];

                TableCell versionCode = new TableCell();
                versionCode.Controls.Add(link);
                versionCode.Text = info[2];

                TableCell date = new TableCell();
                date.Controls.Add(link);
                date.Text = info[3];
                TableCell viewButton = new TableCell();
                Button view = new Button();
                view.Text = "View";
                view.CssClass = "homeButtons";
                view.ID = i.ToString();
                view.Click += new EventHandler(viewReport);
                viewButton.Controls.Add(view);

                row.Cells.Add(apkName);
                row.Cells.Add(packageName);
                row.Cells.Add(versionCode);
                row.Cells.Add(date);
                row.Cells.Add(viewButton);

                allReportsTable.Rows.Add(row);
            }
        }
        protected void signupNav_Click(object sender, EventArgs e)
        {
            Response.Redirect("userProfilePage.aspx");
        }
        protected void viewReport(object sender, EventArgs e)
        {
            int ID = int.Parse(((Button)sender).ID);
            Session["reportID"] = userReports[ID].Key;
            Session["currentReportName"] = apkNames[ID];
            Response.Redirect("reportAnalysisPage.aspx");
        }
        protected void logoutButton_Click(object sender, EventArgs e)
        {
            Session["username"] = "";
            Session["userAccount"] = "";
            Response.Redirect("homePage.aspx");
        }
    }
}