#ifndef __CONTROLEX_H__
#define __CONTROLEX_H__

#include <vector>
#include <math.h>

// category(0)->game(1)->server(2)->room(3)
class GameListUI : public CListUI
{
public:
    struct NodeData
    {
        int _level;
        bool _child_visible;
        CStdString _text;
        CListLabelElementUI* _pListElement;
    };

    class Node
    {
        typedef std::vector <Node*>	Children;
        Children	_children;
        Node*		_parent;
        NodeData    _data;

    private:
        void set_parent(Node* parent) { _parent = parent; }

    public:
        Node() : _parent (NULL) {}
        explicit Node(NodeData t) : _data (t), _parent (NULL) {}
        Node(NodeData t, Node* parent)	: _data (t), _parent (parent) {}
        ~Node() 
        {
            for (int i = 0; i < num_children(); i++)
                delete _children[i]; 
        }
        NodeData& data() { return _data; }	
        int num_children() const { return _children.size(); }
        Node* child(int i)	{ return _children[i]; }
        Node* parent() { return ( _parent);	}
        bool has_children() const {	return num_children() > 0; }
        void add_child(Node* child) 
        {
            child->set_parent(this); 
            _children.push_back(child); 
        }
        void remove_child(Node* child)
        {
            Children::iterator iter = _children.begin();
            for( ; iter < _children.end(); ++iter )
            {
                if( *iter == child ) 
                {
                    _children.erase(iter);
                    return;
                }
            }
        }
        Node* get_last_child()
        {
            if( has_children() )
            {
                return child(num_children() - 1)->get_last_child();
            }
            else return this;
        }
    };	

    GameListUI() : _root(NULL)
    {
        SetItemShowHtml(true);

        _root = new Node;
        _root->data()._level = -1;
        _root->data()._child_visible = true;
        _root->data()._pListElement = NULL;
    }

    ~GameListUI() { if(_root) delete _root; }

    bool Add(CControlUI* pControl)
    {
        if( !pControl ) return false;
        if( _tcscmp(pControl->GetClass(), _T("ListLabelElementUI")) == 0 ) return false;
        return CListUI::Add(pControl);
    }

    bool AddAt(CControlUI* pControl, int iIndex)
    {
        if( !pControl ) return false;
        if( _tcscmp(pControl->GetClass(), _T("ListLabelElementUI")) == 0 ) return false;
        return CListUI::AddAt(pControl, iIndex);
    }

    bool Remove(CControlUI* pControl)
    {
        if( !pControl ) return false;
        if( _tcscmp(pControl->GetClass(), _T("ListLabelElementUI")) == 0 ) return false;
        return CListUI::Remove(pControl);
    }

    bool RemoveAt(int iIndex)
    {
        CControlUI* pControl = GetItemAt(iIndex);
        if( !pControl ) return false;
        if( _tcscmp(pControl->GetClass(), _T("ListLabelElementUI")) == 0 ) return false;
        return CListUI::RemoveAt(iIndex);
    }

    void RemoveAll()
    {
        CListUI::RemoveAll();
        delete _root;
        _root = new Node;
        _root->data()._level = -1;
        _root->data()._child_visible = true;
        _root->data()._pListElement = NULL;
    }

    Node* GetRoot() { return _root; }

    Node* AddNode(LPCTSTR text, Node* parent = NULL)
    {
        if( !parent ) parent = _root;

        CListLabelElementUI* pListElement = new CListLabelElementUI;
        Node* node = new Node;
        node->data()._level = parent->data()._level + 1;
        if( node->data()._level == 0 ) node->data()._child_visible = true;
        else node->data()._child_visible = false;
        node->data()._text = text;
        node->data()._pListElement = pListElement;

        if( !parent->data()._child_visible )
        {
            pListElement->SetVisible(false);
        }
        if( parent != _root && !parent->data()._pListElement->IsVisible() )
        {
            pListElement->SetVisible(false);
        }

        CStdString html_text;
        html_text += _T("<x 6>");
        for( int i = 0; i < node->data()._level; ++i )
        {
            html_text += _T("<x 24>");
        }
        if( node->data()._level < 3 ) {
            if( node->data()._child_visible ) html_text += _T("<a><i tree_expand.png 1></a>");
            else html_text += _T("<a><i tree_expand.png 0></a>");
        }
        html_text += node->data()._text;
        pListElement->SetText(html_text);
        //if( node->data()._level == 0 ) pListElement->SetFixedHeight(28);
        //else pListElement->SetFixedHeight(24);
        pListElement->SetTag((UINT_PTR)node);
        if( node->data()._level == 0 ) {
            pListElement->SetBkImage(_T("file='tree_top.png' corner='2,1,2,1' fade='100'"));
        }

        int index = 0;
        if( parent->has_children() )
        {
            Node* prev = parent->get_last_child();
            index = prev->data()._pListElement->GetIndex() + 1;
        }
        else 
        {
            if( parent == _root ) index = 0;
            else index = parent->data()._pListElement->GetIndex() + 1;
        }
        if( !CListUI::AddAt(pListElement, index) ) {
            delete pListElement;
            delete node;
            node = NULL;
        }

        parent->add_child(node);
        return node;
    }

    void RemoveNode(Node* node)
    {
        if( !node || node == _root ) return;

        for( int i = 0; i < node->num_children(); ++i )
        {
            Node* child = node->child(i);
            RemoveNode(child);
        }

        CListUI::Remove(node->data()._pListElement);
        delete node->data()._pListElement;
        node->parent()->remove_child(node);
        delete node;
    }

    void SetChildVisible(Node* node, bool visible)
    {
        if( !node || node == _root ) return;

        if( node->data()._child_visible == visible ) return;
        node->data()._child_visible = visible;

        CStdString html_text;
        html_text += _T("<x 6>");
        for( int i = 0; i < node->data()._level; ++i )
        {
            html_text += _T("<x 24>");
        }
        if( node->data()._level < 3 ) {
            if( node->data()._child_visible ) html_text += _T("<a><i tree_expand.png 1></a>");
            else html_text += _T("<a><i tree_expand.png 0></a>");
        }
        html_text += node->data()._text;
        node->data()._pListElement->SetText(html_text);

        if( !node->data()._pListElement->IsVisible() ) return;
        if( !node->has_children() ) return;

        Node* begin = node->child(0);
        Node* end = node->get_last_child();
        for( int i = begin->data()._pListElement->GetIndex(); i <= end->data()._pListElement->GetIndex(); ++i )
        {
            CControlUI* control = GetItemAt(i);
            if( _tcscmp(control->GetClass(), _T("ListLabelElementUI")) == 0 )
            {
                if( !visible ) 
                {
                    control->SetVisible(false);
                }
                else
                {
                    Node* local_parent = ((GameListUI::Node*)control->GetTag())->parent();
                    if( local_parent->data()._child_visible && local_parent->data()._pListElement->IsVisible() )
                    {
                        control->SetVisible(true);
                    }
                }
            }
        }
    }

    SIZE GetExpanderSizeX(Node* node) const
    {
        if( !node || node == _root ) return CSize();
        if( node->data()._level >= 3 ) return CSize();

        SIZE szExpander = {0};
        szExpander.cx = 6 + 24 * node->data()._level - 4/*�ʵ��Ŵ�һ��*/;
        szExpander.cy = szExpander.cx + 16 + 8/*�ʵ��Ŵ�һ��*/;
        return szExpander;
    }

private:
    Node* _root;
};

inline double CalculateDelay(double state) {
    if(state < 0.5) {
        return pow(state * 2, 2) / 2.0;
    }
    return 1.0 - (pow((state - 1.0) * 2, 2) / 2.0);
}

class DeskListUI : public CTileLayoutUI
{
public:
    enum { SCROLL_TIMERID = 10 };

    DeskListUI() : m_uButtonState(0), m_dwDelayDeltaY(0), m_dwDelayNum(0), m_dwDelayLeft(0)
    {
        for(int i = 0; i < 500; ++i) 
        {
            CDialogBuilder builder;
            CContainerUI* pDesk = static_cast<CContainerUI*>(builder.Create(_T("desk.xml"), (UINT)0));
            if( pDesk ) {
                this->Add(pDesk);
                TCHAR indexBuffer[16];
                CStdString strIndexString = _T("- ");
                strIndexString += _itot(i+1, indexBuffer, 10);
                strIndexString += _T(" -");
                pDesk->GetItemAt(3)->SetText(strIndexString);
            }
            else {
                this->RemoveAll();
                return;
            }
        }
    }

    void Event(TEventUI& event) 
    {
        if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
            if( m_pParent != NULL ) m_pParent->Event(event);
            else CTileLayoutUI::Event(event);
            return;
        }

        if( event.Type == UIEVENT_TIMER && event.wParam == SCROLL_TIMERID )
        {
            if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
                POINT pt = m_pManager->GetMousePos();
                LONG cy = (pt.y - m_ptLastMouse.y);
                m_ptLastMouse = pt;
                SIZE sz = GetScrollPos();
                sz.cy -= cy;
                SetScrollPos(sz);
            }
            else if( m_dwDelayLeft > 0 ) {
                --m_dwDelayLeft;
                SIZE sz = GetScrollPos();
                if( sz.cy == 0 || sz.cy == GetScrollRange().cy) m_pManager->KillTimer(this, SCROLL_TIMERID);
                sz.cy -= (DWORD)(CalculateDelay((double)m_dwDelayLeft / m_dwDelayNum) * m_dwDelayDeltaY);
                SetScrollPos(sz);
            }
            else {
                m_pManager->KillTimer(this, SCROLL_TIMERID);
            }
            return;
        }
        if( event.Type == UIEVENT_BUTTONDOWN && IsEnabled() )
        {
            m_uButtonState |= UISTATE_CAPTURED;
            m_ptLastMouse = event.ptMouse;
            m_dwDelayDeltaY = 0;
            m_dwDelayNum = 0;
            m_dwDelayLeft = 0;
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
            m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
            return;
        }
        if( event.Type == UIEVENT_BUTTONUP )
        {
            if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
                m_uButtonState &= ~UISTATE_CAPTURED;
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
                if( m_ptLastMouse.y != event.ptMouse.y ) {
                    m_dwDelayDeltaY = (event.ptMouse.y - m_ptLastMouse.y);
                    if( m_dwDelayDeltaY > 120 ) m_dwDelayDeltaY = 120;
                    else if( m_dwDelayDeltaY < -120 ) m_dwDelayDeltaY = -120;
                    m_dwDelayNum = sqrt((double)abs(m_dwDelayDeltaY)) * 5;
                    m_dwDelayLeft = m_dwDelayNum;
                }
                else 
                    m_pManager->KillTimer(this, SCROLL_TIMERID);
            }
            return;
        }

        CTileLayoutUI::Event(event);
    }

    void SetPos(RECT rc)
    {
        if( GetCount() > 0 ) {
            int iDeskWidth = GetItemAt(0)->GetFixedWidth();
            int column = (rc.right - rc.left) / iDeskWidth;
            if( column < 1 ) column = 1;
            SetColumns(column);
        }

        CTileLayoutUI::SetPos(rc);
    }

private:
    UINT m_uButtonState;
    POINT m_ptLastMouse;
    LONG m_dwDelayDeltaY;
    DWORD m_dwDelayNum;
    DWORD m_dwDelayLeft;
};


class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
    CControlUI* CreateControl(LPCTSTR pstrClass) 
    {
        if( _tcscmp(pstrClass, _T("GameList")) == 0 ) return new GameListUI;
        else if( _tcscmp(pstrClass, _T("DeskList")) == 0 ) return new DeskListUI;
        return NULL;
    }
};


#endif __CONTROLEX_H__