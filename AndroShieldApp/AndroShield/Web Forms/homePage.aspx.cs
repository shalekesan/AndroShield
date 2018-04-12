﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Configuration;
using System.Web.UI;
using System.Web.UI.WebControls;


namespace AndroApp
{
    public partial class homepage : System.Web.UI.Page
    {
        databaseLayer androDatabase;
        userAccountTable userAccount;
        protected void Page_Load(object sender, EventArgs e)
        {
            if (!IsPostBack)
            {
                androDatabase = new databaseLayer();
                userAccount = new userAccountTable();
            }
        }
        protected void btn_Click(object sender, EventArgs e)
        {
            Response.Redirect("signUpPage.aspx");
        }
        protected void Button1_Click(object sender, EventArgs e)
        {
            Session["tempUsername"] = emailTxt.Text.ToString();
            Session["password"] = passwordTxt.Text.ToString();

            userAccount = userAccountTable.userLogin(Session["tempUsername"].ToString(), Session["password"].ToString());
            if(userAccount!=null)
            {
                Session["username"] = Session["tempUsername"];
                Session["userAccount"] = userAccount;
                Response.Redirect("userHomePage.aspx");
            }
            else
            {
                Response.Redirect("incorrectCredentialsPage.aspx");
            }
        }

        protected void signupButton_Click(object sender, EventArgs e)
        {
            Response.Redirect("signUpPage.aspx");
        }

        protected void navSignUp_Click(object sender, EventArgs e)
        {
            Response.Redirect("signUpPage.aspx");
        }
    }
}