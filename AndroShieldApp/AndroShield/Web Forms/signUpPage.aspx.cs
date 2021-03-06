﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace AndroApp.Web_Forms
{
    public partial class signUpPage : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            if(!IsPostBack)
            {
                if (Request.UrlReferrer == null)
                    Response.Redirect("homePage.aspx");
            }
        }

        protected void signUpButton_Click(object sender, EventArgs e)
        {
            Session["firstName"] = firstNameTxt.Text.ToString();
            Session["lastName"] = lastNameTxt.Text.ToString();
            Session["tempUsername"] = emailTxt.Text.ToString();
            Session["tempPassword"] = passwordTxt.Text.ToString();
            Session["lastLogin"] = DateTime.Now;
            Session["userAccountCreated"] = userAccountTable.createUserAccount(Session["tempUsername"].ToString(), Session["tempPassword"].ToString(), Session["firstName"].ToString(), Session["lastName"].ToString(), (DateTime)Session["lastLogin"], -1);
            if ((bool)Session["userAccountCreated"])
            {
                takenEmailLabel.Visible = false;
                Session.Abandon();
                Response.Redirect("homePage.aspx");
            }
            else
            {
                takenEmailLabel.Visible = true;
            }
        }

        protected void loginButton_Click(object sender, EventArgs e)
        {
            Response.Redirect("homePage.aspx");
        }
    }
}