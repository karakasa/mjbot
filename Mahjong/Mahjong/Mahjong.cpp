// Mahjong.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "PublicFunction.h"
#include "SyanTen.h"
#include "TenpaiAkariJudge.h"
#include "YamaControl.h"
#include "MemoryLeakMonitor.h"
#include "EventBus.h"
#include "Matching.h"

// Unit Test
#include "YPBasicToolBox.h"

using namespace std;

class MyApp : public wxApp
{
public:
	virtual bool OnInit(); 
};
class MyFrame : public wxFrame { public:    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size); private:    void OnHello(wxCommandEvent& event);    void OnExit(wxCommandEvent& event);    void OnAbout(wxCommandEvent& event);    wxDECLARE_EVENT_TABLE(); };
enum { ID_Hello = 1 };

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Hello, MyFrame::OnHello)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

bool WINAPI ss()
{
	CallWindowProc(NULL, );
	return true;
}

bool MyApp::OnInit()
{
	MyFrame *frame = new MyFrame("the Mahjong", wxPoint(50, 50), wxSize(450, 340));
	frame->Show(true);
	return true; 
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar();
	SetStatusText("Welcome to wxWidgets!");
}

void MyFrame::OnExit(wxCommandEvent& event) { Close(true); }

void MyFrame::OnAbout(wxCommandEvent& event) { wxMessageBox("This is a wxWidgets' Hello world sample", "About Hello World", wxOK | wxICON_INFORMATION); }

void MyFrame::OnHello(wxCommandEvent& event) { wxLogMessage("Hello world from wxWidgets!"); }



int test()
{
	Syanten engineSyanten;
	taj engineTaj;
	string paiStr;
	pai pais[20];
	int paiSafeLen = 16;

	bool simu = false;

	utility::prepareSystemYakuProvider();

	while (true)
	{
		paiSafeLen = 18;
		cin >> paiStr;
		if (paiStr == "exit")
			break;
		if (paiStr == "simu")
		{
			simu = true;
			break;
		}
		int paiLen = convertPaiStringPrepare(paiStr);
		if(paiLen <= 0 || paiLen > 14)
		{
			cerr << "非法的输入。" << endl;
		}
		else {
			switch (paiLen % 3)
			{
				case 1:
					sort(pais, pais + paiLen, paiSort);
					if(convertPaiString(paiStr, pais, &paiSafeLen))
					{
						int k = engineSyanten.calculateSyanten(pais, paiLen);
						if (k > 0)
						{
							cout << k << " 向听。" << endl;
						}
						else {
							cout << "听牌：";
							unordered_set<pai> jrs;
							jrs = std::move(engineTaj.tenpaiDetect(pais, paiLen));
							for (auto& p : jrs)
								cout << (int)(p.fig) << p.type << " ";
							cout << endl;
							cout << "效率: " << engineTaj.effeci_cnt;
							cout << endl;
						}
					}
					break;
				case 2:
				{
					bool failed = false;
					judgeRequest jrq;
					convertPaiString(paiStr, pais, &paiSafeLen);
					jrq.tgtpai = pais[paiLen - 1];
					jrq.fulucnt = 4 - (paiLen - 2) / 3;
					string fulud;
					int fuludCnt = 4;
					pai fulu[4];
					jrq.paicnt = paiLen - 1;
					copy(pais, pais + paiLen - 1, jrq.pais);
					for (int i = 0; i < jrq.fulucnt; i++)
					{
						cin >> fulud;
						fuludCnt = convertPaiStringPrepare(fulud);
						if (fuludCnt != 3 && fuludCnt != 4)
						{
							cout << "副露 " << i << " 不合法。" << endl;
							failed = true;
							break;
						}
						int fuludCntReserved = fuludCnt;
						convertPaiString(fulud, fulu, &fuludCnt);
						if (fuludCntReserved == 4)
						{
							cout << "暗杠与否？(y/n)";
							char p;
							cin >> p;
							jrq.fulus[i].type = (p == 'y') ? mentsu_KEZ_KANG_S : mentsu_KEZ_KANG_A;
							jrq.fulus[i].start = jrq.fulus[i].middle = jrq.fulus[i].last = fulu[0];
							cout << endl;
						}
						else {
							if (fulu[0] == fulu[1])
							{
								jrq.fulus[i].type = mentsu_KEZ_PON_A;
								jrq.fulus[i].start = jrq.fulus[i].middle = jrq.fulus[i].last = fulu[0];
							}
							else {
								jrq.fulus[i].type = mentsu_SHUNZ_CHI_A;
								jrq.fulus[i].start = fulu[0];
								jrq.fulus[i].middle = fulu[1];
								jrq.fulus[i].last = fulu[2];
							}
						}
					}
					if (failed && jrq.fulucnt != 0)
					{
						break;
					}
					cout << "荣和与否？(y/n)";
					char p;
					cin >> p;
					cout << endl;
					jrq.akariStatus = (p == 'y') ? RON : TSUMO;
					yakuTable yt = engineTaj.yakuDetect(jrq);
					cout << yt.yakutotal << " 翻 " << yt.huutotal << " 符 " << yt.basicpt << " 基本点" << endl;
					char w[255];
					memset(w, 0, sizeof(w));
					for (auto& yk : yt.yakus)
					{
						engineTaj.currentProvider->queryName(yk.yakuid, yk.yakusubid, w, 250);
						cout << w << " " << yk.pt << " 翻" << endl;
					}
					cout << "效率: " << engineTaj.effeci_cnt;
					cout << endl;
					break;
				}
				default:
					cerr << "非法的输入。" << endl;
					break;
			}
		}
	}

	if (simu)
	{
		int p = GetTickCount();
		SimpleAI sai[4];
		match* testRun = new match();
		for (int i = 0; i < 4; i++)
			testRun->core->assignClient(i, &(sai[i]));
		testRun->core->run();
		testRun->core->receiveEvent(NULL, 1);
		testRun->core->waitUntilEnd();
		delete testRun;
		p = GetTickCount() - p;
		cout << endl << p << endl;
		cin >> paiSafeLen;
	}

    return MemoryLeakMonitor::gc();
}

