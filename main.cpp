#include <iostream>
#include <windows.h>
#include <mysql.h>
#include <string>
#include <cmath>
#include <iomanip>
#include <ctime>
using namespace std;

MYSQL* con;
MYSQL_RES *res;
MYSQL_ROW row;

struct node
{
    string custid;
    float balance;
    struct node *l;
    struct node *r;
    int h;
}*root;
int height(struct node *root)
{
    if(root==NULL)
        return -1;
    else
    {
        int lh=height(root->l);
        int rh=height(root->r);
        if(lh>rh)
            return lh+1;
        else
            return rh+1;
    }
}
struct node *rightr(struct node *root)
{
    struct node *b=root->l;
    struct node *c=b->r;
    root->l=c;
    b->r=root;
    b->h=height(b);
    root->h=height(root);
    root=b;
    return root;
}
struct node *leftr(struct node *root)
{
    struct node *b=root->r;
    struct node *c=b->l;
    root->r=c;
    b->l=root;
    b->h=height(b);
    root->h=height(root);
    root=b;
    return root;
}
struct node *lr(struct node *root)
{
    root->l=leftr(root->l);
    root=rightr(root);
    return root;
}
struct node *rl(struct node *root)
{
    root->r=rightr(root->r);
    root=leftr(root);
    return root;
}
struct node *ins(struct node *root,struct node *k)
{
    if(root==NULL)
        return k;
    if(k->custid<root->custid)
    {
        if(root->l==NULL)
            root->l=k;
        else
            root->l=ins(root->l,k);
    }
    else
    {
        if(root->r==NULL)
            root->r=k;
        else
            root->r=ins(root->r,k);
    }
    root->h=height(root);
    int bal=height(root->l)-height(root->r);
    if(bal>1&&k->custid<root->l->custid)
        root=rightr(root);
    if(bal<-1&&k->custid>root->r->custid)
        root=leftr(root);
    if(bal>1&&k->custid>root->l->custid)
        root=lr(root);
    if(bal<-1&&k->custid<root->r->custid)
        root=rl(root);
    return root;
}
struct node *fetchalldata(struct node *root,string t)
{
    mysql_query(con,("select CustomerID,balance from "+t+"").c_str());
    res=mysql_use_result(con);
    while(row=mysql_fetch_row(res))
    {
        struct node *s=new struct node;
        s->custid=row[0];
        s->balance=atoi(row[1]);
        s->l=NULL;
        s->r=NULL;
        s->h=0;
        root=ins(root,s);
    }
    mysql_free_result(res);
    return root;
}
struct node *sear(struct node *root,string k)
{
    if(root==NULL)
        return NULL;
    if(root->custid==k)
        return root;
    else if(k<root->custid)
        return sear(root->l,k);
    else if(k>root->custid)
        return sear(root->r,k);
}
void updatealldata(struct node *root,string t)
{
    if(root!=NULL)
    {
        mysql_query(con,("update "+t+" set balance='"+to_string(root->balance)+"' where CustomerID='"+root->custid+"'").c_str());
        updatealldata(root->l,t);
        updatealldata(root->r,t);
    }
}

struct THistory
{
    public:
    string custid;
    string type;
    float amount;
    string datetime;
};

template <typename t>
class stack
{
    public:
    int f=-1;
    t a[1000];
    void push(THistory s)
    {
        f++;
        a[f]=s;
    }
    void pop()
    {
        f--;
    }
    bool empty()
    {
        if(f==-1)
            return 1;
        else
            return 0;
    }
    t top()
    {
        return a[f];
    }
};
stack <THistory> th1;
stack <THistory> th2;
stack <THistory> fetchallhistory(stack <THistory> th1,string cid)
{
    mysql_query(con,("select * from thistory where CustomerID='"+cid+"'").c_str());
    res=mysql_use_result(con);
    while(row=mysql_fetch_row(res))
    {
        struct THistory s;
        s.custid=row[0];
        s.type=row[1];
        s.amount=atoi(row[2]);
        s.datetime=row[3];
        th1.push(s);
    }
    mysql_free_result(res);
    return th1;
}
void updateallhistory(stack <THistory> th2)
{
    while(!th2.empty())
    {
        struct THistory st=th2.top();
        mysql_query(con,("insert into thistory(CustomerID, Type, Amount, Date_Time) values('"+st.custid+"','"+st.type+"','"+to_string(st.amount)+"','"+st.datetime+"')").c_str());
        th2.pop();
    }
}

struct vertex
{
	string city;
	string place;
	int d;
	struct vertex *next;
}**a,*temp;
struct vertex *newnode(string c,string v,int d)
{
	struct vertex *s=new vertex;
	s->city=c;
	s->place=v;
	s->d=d;
	s->next=NULL;
	return s;
}
struct vertex *inse(struct vertex *head,string c,string e,int d)
{
	for(temp=head;temp->next!=NULL;temp=temp->next)
	{}
	temp->next=newnode(c,e,d);
	return head;
}
class graph
{
	public:
	int n;
	struct vertex **a;
	graph(int x)
    {
    	n=x;
    	a=new vertex*[n];
    	string u,y;
        a[0]=newnode("Delhi","Dwarka",0);
        a[1]=newnode("Delhi","AnandVihar",0);
        a[2]=newnode("Delhi","Rohini",0);
    }
	int getin(string c,string v)
	{
    	for(int i=0;i<n;i++)
    	{
        	if(a[i]->city==c&&a[i]->place==v)
            	return i;
    	}
    	return -1;
	}
	void addedge(string c,string v,string e,int d)
	{
        int q=getin(c,v);
        a[q]=inse(a[q],c,e,d);
        int t=getin(c,e);
    	a[t]=inse(a[t],c,v,d);
	}
	void nearestnode(string c,string v)
	{
	    int t=getin(c,v);
	    int o=0;
	    string f;
	    for(temp=a[t];temp!=NULL;temp=temp->next)
        {
            if(temp->d>o)
            {
                o=temp->d;
                f=temp->place;
            }
        }
        cout<<"\nNearest Branch is: "<<f;
        cout<<"\nDistance from origin is: "<<o<<endl<<endl;
	}
};

graph initializegraph()
{
    graph g(3);
    g.addedge("Delhi","Rohini","Dwarka",7);
    g.addedge("Delhi","Rohini","AnandVihar",15);
    g.addedge("Delhi","AnandVihar","Dwarka",22);
    return g;
}
void findnearestbranch()
{
    string c,p;
    cout<<"\n-----------------------Find Nearest Bank Branch-----------------------\n\n";
    cout<<"Enter your City: ";
    cin>>c;
    cout<<"Enter your Locality: ";
    cin>>p;
    graph g(3);
    g=initializegraph();
    g.nearestnode(c,p);
}




string now()
{
    time_t n=time(0);
    tm *ltm=localtime(&n);
    string cd=to_string(ltm->tm_year+1900)+'-'+to_string(ltm->tm_mon+1)+'-'+to_string(ltm->tm_mday);
    string ct=to_string(ltm->tm_hour)+':'+to_string(ltm->tm_min)+':'+to_string(ltm->tm_sec);
    string cdt=cd+" "+ct;
    return cdt;
}

class account
{
    public:
    string pass;
    string custid;
    string fname;
    string lname;
    string dob;
    string email;
    string mob;
    string add;
    string id;
    string idnum;
    string debitcard;
    float balance;
    public:
    virtual void mainMenu()=0;
    virtual void logout()=0;
    void newUser()
    {
        cout<<"\nEnter the Following Details Carefully: \n\n";
        cout<<"Enter your First Name: ";
        cin>>fname;
        cout<<"Enter your Last Name: ";
        cin>>lname;
        x:
        try
        {
            cout<<"Enter your Date of Birth in YYYY-MM-DD format: ";
            cin>>dob;
            if(dob[3]!='-'&&dob[4]!='-'&&dob[7]!='-'&&dob.length()!=10)
                throw 1;
        }
        catch(int a)
        {
            cout<<"\n                !!Incorrect Format of Date Entered!!         \n";
            cout<<"Enter Again\n";
            goto x;
        }
        y:
        try
        {
            cout<<"Enter your Email Address Eg: aa@gmail.com: ";
            cin>>email;
            if(email.find('@')==string::npos||email.find(".com")==string::npos)
                throw "ERROR";
        }
        catch(const char *b)
        {
            cout<<"\n              !!Incorrect Format of Email ID Entered!!\n";
            cout<<"Enter Again\n";
            goto y;
        }
        z:
        try
        {
            cout<<"Enter your Mobile Number: ";
            cin>>mob;
            if(mob.length()!=10)
                throw "ERROR";
        }
        catch(const char* c)
        {
            cout<<"                 !!Mobile Number should be of 10 DIGITS!!\n";
            cout<<"Enter Again\n";
            goto z;
        }
        cout<<"Enter your Address: ";
        getline(cin>>ws,add);
        cout<<"Enter the Type of ID Proof: ";
        getline(cin,id);
        cout<<"Enter your ID number: ";
        cin>>idnum;
        cout<<"\n---------------------Details Submitted Successfully-------------------\n\n";
        custid="qwertyui";
        custid[0]=fname[0];
        custid[1]=lname[0];
        custid[2]=dob[2];
        custid[3]=dob[3];
        custid[4]=mob[6];
        custid[5]=mob[7];
        custid[6]=mob[8];
        custid[7]=mob[9];
        cout<<"Your Customer ID is :"<<custid<<endl;
        w:
        try
        {
            cout<<"Enter the Password you want to set: ";
            cin>>pass;
            if(pass.length()<8)
                throw 0;
        }
        catch(int d)
        {
            cout<<"!!ERROR!! Password should be At Least of 8 characters.\n";
            cout<<"Enter Again\n";
            goto w;
        }
    }
    void deposit(float amn)
    {
        balance=balance+amn;
        int j;
        struct THistory s;
        s.custid=custid;
        s.type="Credit";
        s.amount=amn;
        s.datetime=now();
        th2.push(s);
        cout<<"\n---------------------Amount Deposited Successfully--------------------\n\n";
        k:
        cout<<"1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>j;
        switch(j)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto k;
        }
    }
    void withdraw(float amo)
    {
        int j;
        if(amo>balance)
            cout<<"-----------------------!!Insufficient Balance!!-----------------------\n\n";
        else
        {
            balance=balance-amo;
            struct THistory s;
            s.custid=custid;
            s.type="Debit";
            s.amount=amo;
            s.datetime=now();
            th2.push(s);
            cout<<"---------------------Amount Withdrawn Successfully--------------------\n\n";
        }
        k:
        cout<<"1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>j;
        switch(j)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto k;
        }
    }
    void viewBalance()
    {
        int j;
        cout<<"\nAvailable Balance is: Rs. "<<balance<<endl;
        k:
        cout<<"\n1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>j;
        switch(j)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto k;
        }
    }
    void viewDetails()
    {
        int j;
        cout<<"Account Details are: \n\n";
        cout<<"First Name: "<<fname<<endl;
        cout<<"Last Name: "<<lname<<endl;
        cout<<"Date of Birth: "<<dob<<endl;
        cout<<"Registered Mobile Number: "<<mob<<endl;
        cout<<"Registered Email ID: "<<email<<endl;
        cout<<"Address: "<<add<<endl;
        cout<<"KYC Status: ";
        if(id.length()==0)
            cout<<"Not Done";
        else
            cout<<"Done";
        k:
        cout<<"\n\n1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>j;
        switch(j)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto k;
        }
    }
    void modifyDetails()
    {
        int n;
        string update;
        char ch='y';
        while(ch=='y'||ch=='Y')
        {
            v:
            cout<<"Select the field you want to modify: \n";
            cout<<"1. First Name\n";
            cout<<"2. Last Name\n";
            cout<<"3. Date of Birth\n";
            cout<<"4. Email-ID\n";
            cout<<"5. Mobile Number\n";
            cout<<"6. Address\n";
            cout<<"7. KYC\n";
            cout<<"\nEnter your Choice: ";
            cin>>n;
            cout<<"!!Note: CHANGES IN DETAILS WILL ONLY BE VISIBLE ONCE YOU SUBMIT \nTHE UPDATED KYC DETAILS IN YOUR NEAREST BRANCH!!\n\n";
            switch(n)
            {
                case 1:
                    cout<<"Enter the updated first name: ";
                    cin>>update;
                    fname=update;
                    break;
                case 2:
                    cout<<"Enter the updated last name: ";
                    cin>>update;
                    lname=update;
                    break;
                case 3:
                    cout<<"Enter the updated Date Of Birth: ";
                    cin>>update;
                    dob=update;
                    break;
                case 4:
                    cout<<"Enter the updated Email-ID: ";
                    cin>>update;
                    email=update;
                    break;
                case 5:
                    cout<<"Enter the updated Mobile Number: ";
                    cin>>update;
                    mob=update;
                    break;
                case 6:
                    cout<<"Enter the updated Address: ";
                    cin>>update;
                    add=update;
                    break;
                case 7:
                    if(id.length()!=0)
                        cout<<"----------KYC ALREADY DONE------------\n\n";
                    else
                        cout<<"Submit the KYC Details At Your Nearest Branch To Complete KYC.";
                    break;
                default:
                    cout<<"\n!!WRONG VALUE ENTERED!!\n\n";
                    goto v;
                    break;
            }
            cout<<"Do you want to Modify More Details (y/n): ";
            cin>>ch;
        }
        int j;
        k:
        cout<<"\n\n1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>j;
        switch(j)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto k;
        }
    }
    void fundtransfer()
    {
        string c,t;
        float amt;
        cout<<"\n-------------------------Fund Transfer Portal-------------------------\n\n";
        cout<<"Enter Beneficiary Customer ID : ";
        cin>>c;
        cout<<"Enter Beneficiary Account Type(current/savings) : ";
        cin>>t;
        cout<<"Enter Amount : ";
        cin>>amt;
        root=fetchalldata(root,t);
        balance-=amt;
        struct THistory s1;
        s1.custid=custid;
        s1.type="Debit";
        s1.amount=amt;
        s1.datetime=now();
        th2.push(s1);
        root=sear(root,c);
        root->balance+=amt;
        struct THistory s2;
        s2.custid=c;
        s2.type="Credit";
        s2.amount=amt;
        mysql_query(con,("insert into thistory(CustomerID, Type, Amount) values('"+s2.custid+"','"+s2.type+"','"+to_string(s2.amount)+"')").c_str());
        cout<<"\n--------------------Fund Transferred Successfully---------------------\n\n";
        int g;
        l:
        cout<<"1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>g;
        switch(g)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto l;
        }
    }
    void viewthistory()
    {
        stack <THistory> t1=th1;
        stack <THistory> t2=th2;
        cout<<"\n-------------------------Transaction History--------------------------\n\n";
        cout<<"+----------------------+------------+-------------------+\n";
        cout<<"| Type of Transaction  |   Amount   |    Date & Time    |\n";
        cout<<"+----------------------+------------+-------------------+\n";
        while(!t2.empty())
        {
            struct THistory s=t2.top();
            cout<<"| "<<left<<setw(21)<<s.type<<"|"<<setw(12)
                <<s.amount<<"|"<<setw(19)<<s.datetime<<"|"<<endl;
            t2.pop();
        }
        while(!t1.empty())
        {
            struct THistory s=t1.top();
            cout<<"| "<<left<<setw(21)<<s.type<<"|"<<setw(12)
                <<s.amount<<"|"<<setw(19)<<s.datetime<<"|"<<endl;
            t1.pop();
        }
        cout<<"+----------------------+------------+-------------------+\n\n";
        int g;
        l:
        cout<<"1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>g;
        switch(g)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto l;
        }
    }
};
class current: public account
{
    public:
    void issueCHQ()
    {
        char ch;
        int j;
        cout<<"Note: Rs. 100 will be Deducted from your Account as Service Charges.\n\n";
        cout<<"Are you sure you want to Issue a New Cheque Book (y/n): ";
        cin>>ch;
        if(ch=='y'||ch=='Y')
        {
            cout<<"--------------------CHEQUE BOOK ISSUED SUCCESSFULLY-------------------\n\n";
            cout<<"It will be delivered to your Address within 3-5 Working Days.\n\n";
            mysql_query(con,("update current set ChequeBook='Issued' where CustomerID='"+custid+"'").c_str());
            withdraw(100);
        }
        else
            cout<<"!!Program Aborted!!\n\n";
        k:
        cout<<"1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>j;
        switch(j)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto k;
        }
    }
    void chargesImp()
    {
        int g;
        if(balance<1000)
        {
            cout<<"Charges Imposed are: \n";
            cout<<"Rs. 100 as low balance in account. \n";
        }
        else
            cout<<"No Charges Imposed.\n\n";
        l:
        cout<<"1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>g;
        switch(g)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto l;
        }
    }
    void logout()
    {
        cout<<"\n\n-------------------------------THANK YOU------------------------------\n\n";
        updatealldata(root,"current");
        updateallhistory(th2);
        mysql_query(con,("update current set FirstName='"+fname+"',Lastname='"+lname+"',DOB='"+dob+"',Email='"+email+"',MobileNo='"+mob+"',Address='"+add+"',IDType='"+id+"',IDNum='"+idnum+"',Balance='"+to_string(balance)+"' where CustomerID='"+custid+"'").c_str());
        exit(1);
    }
    void mainMenu()
    {
        int n;
        float amn,amo;
        u:
        cout<<"\n1. Deposit Money\n";
        cout<<"2. Withdraw Money\n";
        cout<<"3. View Account Balance\n";
        cout<<"4. Issue Cheque Book\n";
        cout<<"5. Fund Transfer\n";
        cout<<"6. View Transaction History\n";
        cout<<"7. View Charges Imposed\n";
        cout<<"8. View Account Details\n";
        cout<<"9. Modify Account Details\n";
        cout<<"10. Logout\n";
        cout<<"\nEnter your Choice: ";
        cin>>n;
        switch(n)
        {
            case 1:
                cout<<"Enter the Amount to be Deposited: Rs. ";
                cin>>amn;
                deposit(amn);
                break;
            case 2:
                cout<<"Enter the Amount to be Withdrawn: Rs. ";
                cin>>amo;
                withdraw(amo);
                break;
            case 3:
                viewBalance();
                break;
            case 4:
                issueCHQ();
                break;
            case 5:
                fundtransfer();
                break;
            case 6:
                viewthistory();
                break;
            case 7:
                chargesImp();
                break;
            case 8:
                viewDetails();
                break;
            case 9:
                modifyDetails();
                break;
            case 10:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto u;
        }
    }
    void login()
    {
        string uname,pas;
        j:
        cout<<"\nEnter Login Credentials: \n\n";
        cout<<"Enter Customer ID: ";
        cin>>uname;
        cout<<"Enter Password: ";
        cin>>pas;
        mysql_query(con,("select * from current where CustomerID='"+uname+"' and password='"+pas+"'").c_str());
        res=mysql_use_result(con);
        row=mysql_fetch_row(res);
        if(row!=NULL)
        {
            custid=row[0];
            pass=row[1];
            fname=row[2];
            lname=row[3];
            dob=row[4];
            email=row[5];
            mob=row[6];
            add=row[7];
            id=row[8];
            idnum=row[9];
            balance=atoi(row[10]);
        }
        else
        {
            cout<<"\n                    !!INCORRECT LOGIN CREDENTIALS!!\n";
            cout<<"Enter Correct Login Credentials.\n\n";
            goto j;
        }
        mysql_free_result(res);
        th1=fetchallhistory(th1,custid);
    }
};
class savings: public account
{
    public:
    void loan()
    {
        int n,t,g;
        float p,r;
        char ch;
        o:
        cout<<"\nSelect the Type of Loan: \n\n";
        cout<<"1. House Loan\n";
        cout<<"2. Personal Loan\n";
        cout<<"3. Education Loan\n";
        cout<<"4. Agricultural Loan\n";
        cout<<"5. Vehicle Loan\n";
        cout<<"6. Gold Loan\n";
        cout<<"\nEnter Your Choice: ";
        cin>>n;
        cout<<"\nEnter the Loan Amount: ";
        cin>>p;
        cout<<"Enter Number of Years you are taking loan for: ";
        cin>>t;
        t=t*12;
        switch(n)
        {
            case 1:
                cout<<"\n           ......RATE OF INTEREST ON HOUSE LOANS IS 6%......\n\n";
                r=0.06;
                break;
            case 2:
                cout<<"\n       ......RATE OF INTEREST ON PERSONAL LOANS IS 11%......\n\n";
                r=0.11;
                break;
            case 3:
                cout<<"\n       ......RATE OF INTEREST ON EDUCATION LOANS IS 5%......\n\n";
                r=0.05;
                break;
            case 4:
                cout<<"\n    ......RATE OF INTEREST ON AGRICULTURAL LOANS IS 7%......\n\n";
                r=0.07;
                break;
            case 5:
                cout<<"\n         ......RATE OF INTEREST ON VEHICLE LOANS IS 8%......\n\n";
                r=0.08;
                break;
            case 6:
                cout<<"\n           ......RATE OF INTEREST ON GOLD LOANS IS 9%......\n\n";
                r=0.09;
                break;
            default:
                cout<<"\n!!WRONG VALUE ENTERED!!";
                goto o;
        }
        float emi=(p*r*pow((r+1),t))/(pow((r+1),(t-1)));
        cout<<"Equated Monthly Installment (EMI) is: "<<emi<<endl;
        cout<<"\nAre you sure you want to take the LOAN (y/n): ";
        cin>>ch;
        if(ch=='y'||ch=='Y')
        {
            cout<<"\nSubmit the Required Documents at your Nearest Branch.\n";
            cout<<"After Verification your Loan Amount will be Credited to your Account.\n\n";
        }
        else
        {
            cout<<"\n!!Program Aborted!!\n\n";
        }
        l:
        cout<<"1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>g;
        switch(g)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto l;
        }
    }
    void debitCard()
    {
        char ch;
        int j;
        if(debitcard=="Not Issued")
        {
            cout<<"Note: Rs. 100 will be Deducted from your Account as Service Charges.\n\n";
            cout<<"Are you sure you want to Issue a Debit Card (y/n): ";
            cin>>ch;
            if(ch=='y'||ch=='Y')
            {
                cout<<"\n--------------------DEBIT CARD ISSUED SUCCESSFULLY--------------------\n\n";
                cout<<"It will be delivered to your Address within 3-5 Working Days.\n\n";
                debitcard="Issued";
                withdraw(100);
            }
            else
                cout<<"\n!!Program Aborted!!\n\n";
        }
        else
        {
            cout<<"\n\n-----------------------DEBIT CARD ALREADY ISSUED----------------------\n\n";
        }
        k:
        cout<<"1. To go Back To MAIN MENU\n";
        cout<<"2. LOGOUT\n";
        cout<<"Enter Your Choice: ";
        cin>>j;
        switch(j)
        {
            case 1:
                mainMenu();
                break;
            case 2:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto k;
        }
    }
    void logout()
    {
        cout<<"\n\n-------------------------------THANK YOU------------------------------\n\n";
        updatealldata(root,"savings");
        updateallhistory(th2);
        mysql_query(con,("update savings set FirstName='"+fname+"',Lastname='"+lname+"',DOB='"+dob+"',Email='"+email+"',MobileNo='"+mob+"',Address='"+add+"',IDType='"+id+"',IDNum='"+idnum+"',Balance='"+to_string(balance)+"',DebitCard='"+debitcard+"' where CustomerID='"+custid+"'").c_str());
        exit(1);
    }
    void mainMenu()
    {
        int n;
        float amn,amo;
        u:
        cout<<"\n1. Deposit Money\n";
        cout<<"2. Withdraw Money\n";
        cout<<"3. View Account Balance\n";
        cout<<"4. Fund Transfer\n";
        cout<<"5. Get a Loan\n";
        cout<<"6. View Transaction History\n";
        cout<<"7. Issue Debit Card\n";
        cout<<"8. View Account Details\n";
        cout<<"9. Modify Account Details\n";
        cout<<"10. Logout\n";
        cout<<"\nEnter your Choice: ";
        cin>>n;
        switch(n)
        {
            case 1:
                cout<<"Enter the Amount to be Deposited: Rs. ";
                cin>>amn;
                deposit(amn);
                break;
            case 2:
                cout<<"Enter the Amount to be Withdrawn: Rs. ";
                cin>>amo;
                withdraw(amo);
                break;
            case 3:
                viewBalance();
                break;
            case 4:
                fundtransfer();
                break;
            case 5:
                loan();
                break;
            case 6:
                viewthistory();
                break;
            case 7:
                debitCard();
                break;
            case 8:
                viewDetails();
                break;
            case 9:
                modifyDetails();
                break;
            case 10:
                logout();
                break;
            default:
                cout<<"!!Wrong Value Entered!!";
                goto u;
        }
    }
    void login()
    {
        string uname,pas;
        e:
        cout<<"\nEnter Login Credentials: \n\n";
        cout<<"Enter Customer ID: ";
        cin>>uname;
        cout<<"Enter Password: ";
        cin>>pas;
        mysql_query(con,("select * from savings where CustomerID='"+uname+"' and password='"+pas+"'").c_str());
        res=mysql_use_result(con);
        row=mysql_fetch_row(res);
        if(row!=NULL)
        {
            custid=row[0];
            pass=row[1];
            fname=row[2];
            lname=row[3];
            dob=row[4];
            email=row[5];
            mob=row[6];
            add=row[7];
            id=row[8];
            idnum=row[9];
            balance=atoi(row[10]);
            debitcard=row[11];
        }
        else
        {
            cout<<"\n                    !!INCORRECT LOGIN CREDENTIALS!!\n";
            cout<<"Enter Correct Login Credentials.\n\n";
            goto e;
        }
        mysql_free_result(res);
        th1=fetchallhistory(th1,custid);
    }
};
int main()
{
    con=mysql_init(0);
    con=mysql_real_connect(con,"localhost","root","root","bank",0,NULL,0);
    if(!con)
        cout<<"!!ERROR!! Database not connected.";
    qw:
    int n;
    cout<<"                 DATA STRUCTURES & ALGORITHMS PROJECT\n\n";
    cout<<"                       WELCOME TO BANK OF INDIA  \n\n";
    cout<<"                              Home Page\n\n";
    cout<<"1. New User...Register\n";
    cout<<"2. Existing User...Login\n";
    cout<<"3. Find Nearest Bank Branch...\n";
    p:
    cout<<"\nEnter Your Choice: ";
    cin>>n;
    switch(n)
    {
        case 1:
            int a;
            f:
            cout<<"\nEnter the TYPE of Account you want to Open: \n\n";
            cout<<"1. Current Account\n";
            cout<<"2. Savings Account\n";


            cout<<"\nEnter Your Choice: ";
            cin>>a;
            if(a==1)
            {
                current c;
                c.newUser();
                mysql_query(con,("insert into current(CustomerID,password,FirstName,Lastname,DOB,Email,MobileNo,Address,IDType,IDNum) values('"+c.custid+"','"+c.pass+"','"+c.fname+"','"+c.lname+"','"+c.dob+"','"+c.email+"','"+c.mob+"','"+c.add+"','"+c.id+"','"+c.idnum+"')").c_str());
                system("cls");
                goto qw;
            }
            else if(a==2)
            {
                savings s;
                s.newUser();
                mysql_query(con,("insert into savings(CustomerID,password,FirstName,Lastname,DOB,Email,MobileNo,Address,IDType,IDNum) values('"+s.custid+"','"+s.pass+"','"+s.fname+"','"+s.lname+"','"+s.dob+"','"+s.email+"','"+s.mob+"','"+s.add+"','"+s.id+"','"+s.idnum+"')").c_str());
                system("cls");
                goto qw;
            }
            else
            {
                 cout<<"!!Wrong Value Entered!!\n\n";
                 goto f;
            }
        case 2:
            int b;
            h:
            cout<<"\nEnter your Account Type: \n\n";
            cout<<"1. Current Account\n";
            cout<<"2. Savings Account\n";
            cout<<"\nEnter Your Choice: ";
            cin>>b;
            if(b==1)
            {
                current c;
                c.login();
                c.mainMenu();
            }
            else if(b==2)
            {
                savings s;
                s.login();
                cout<<11;
                s.mainMenu();
            }
            else
            {
                 cout<<"!!Wrong Value Entered!!\n\n";
                 goto h;
            }
        case 3:
            findnearestbranch();
            int g;
            l:
            cout<<"1. To go Back To HOME PAGE\n";
            cout<<"2. EXIT\n";
            cout<<"Enter Your Choice: ";
            cin>>g;
            switch(g)
            {
                case 1:
                    goto qw;
                    break;
                case 2:
                    exit(1);
                    break;
                default:
                    cout<<"!!Wrong Value Entered!!";
                    goto l;
            }
            break;
        default:
            cout<<"!!Wrong Value Entered!!\n";
            goto p;
            break;
    }
}


