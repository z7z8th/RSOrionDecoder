#include "iniparser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sstream>

#include "herr.h"
#include "hfile.h"

IniParser::IniParser() {
    _comment = DEFAULT_INI_COMMENT;
    _delim = DEFAULT_INI_DELIM;
    root_ = NULL;
}

IniParser::~IniParser() {
    Unload();
}

int IniParser::Unload() {
    SAFE_DELETE(root_);
    return 0;
}

int IniParser::LoadFromFile(const char* filepath) {
    _filepath = filepath;

    HFile file;
    if (file.open(filepath, "r") != 0) {
        return ERR_OPEN_FILE;
    }

    hbuf_t buf;
    file.readall(buf);

    return LoadFromMem((const char*)buf.base);
}

int IniParser::LoadFromMem(const char* data) {
    Unload();

    root_ = new IniNode;
    root_->type = IniNode::INI_NODE_TYPE_ROOT;

    std::stringstream ss;
    ss << data;
    std::string strLine;
    int line = 0;
    string::size_type pos;

    string      content;
    string      comment;
    string      strDiv;
    IniNode* pScopeNode = root_;
    IniNode* pNewNode = NULL;
    while (std::getline(ss, strLine)) {
        ++line;

        content = trimL(strLine);
        if (content.length() == 0)  {
            // blank line
            strDiv += '\n';
            continue;
        }

        // trim_comment
        comment = "";
        pos = content.find_first_of(_comment);
        if (pos != string::npos) {
            comment = content.substr(pos);
            content = content.substr(0, pos);
        }

        content = trimR(content);
        if (content.length() == 0) {
            strDiv += strLine;
            strDiv += '\n';
            continue;
        } else if (strDiv.length() != 0) {
            IniNode* pNode = new IniNode;
            pNode->type = IniNode::INI_NODE_TYPE_DIV;
            pNode->label = strDiv;
            pScopeNode->Add(pNode);
            strDiv = "";
        }

        if (content[0] == '[') {
            if (content[content.length()-1] == ']') {
                // section
                content = trim(content.substr(1,content.length()-2));
                pNewNode = new IniNode;
                pNewNode->type = IniNode::INI_NODE_TYPE_SECTION;
                pNewNode->label = content;
                root_->Add(pNewNode);
                pScopeNode = pNewNode;
            } else {
                // hlogw("format error, line:%d", line);
                continue;   // ignore    
            }
        } else {
            pos = content.find_first_of(_delim);
            if (pos != string::npos) {
                // key-value
                pNewNode = new IniNode;
                pNewNode->type = IniNode::INI_NODE_TYPE_KEY_VALUE;
                pNewNode->label = trim(content.substr(0,pos));
                pNewNode->value = trim(content.substr(pos+_delim.length()));
                pScopeNode->Add(pNewNode);
            } else {
                // hlogw("format error, line:%d", line);
                continue;   // ignore
            }
        }

        if (comment.length() != 0) {
            // tail_comment
            IniNode* pNode = new IniNode;
            pNode->type = IniNode::INI_NODE_TYPE_SPAN;
            pNode->label = comment;
            pNewNode->Add(pNode);
            comment = "";
        }
    }

    // file end comment
    if (strDiv.length() != 0) {
        IniNode* pNode = new IniNode;
        pNode->type = IniNode::INI_NODE_TYPE_DIV;
        pNode->label = strDiv;
        root_->Add(pNode);
    }

    return 0;
}

void IniParser::DumpString(IniNode* pNode, string& str) {
    if (pNode == NULL)  return;

    if (pNode->type != IniNode::INI_NODE_TYPE_SPAN) {
        if (str.length() > 0 && str[str.length()-1] != '\n') {
            str += '\n';
        }
    }

    switch (pNode->type) {
    case IniNode::INI_NODE_TYPE_SECTION: {
        str += '[';
        str += pNode->label;
        str += ']';
    }
    break;
    case IniNode::INI_NODE_TYPE_KEY_VALUE: {
        str += asprintf("%s %s %s", pNode->label.c_str(), _delim.c_str(), pNode->value.c_str());
    }
    break;
    case IniNode::INI_NODE_TYPE_DIV: {
        str += pNode->label;
    }
    break;
    case IniNode::INI_NODE_TYPE_SPAN: {
        str += '\t';
        str += pNode->label;
    }
    break;
    default:
    break;
    }

    for (auto p : pNode->children) {
        DumpString(p, str);
    }
}

string IniParser::DumpString() {
    string str;
    DumpString(root_, str);
    return str;
}

int IniParser::Save() {
    return SaveAs(_filepath.c_str());
}

int IniParser::SaveAs(const char* filepath) {
    string str = DumpString();
    if (str.length() == 0) {
        return 0;
    }

    HFile file;
    if (file.open(filepath, "w") != 0) {
        return ERR_SAVE_FILE;
    }
    file.write(str.c_str(), str.length());

    return 0;
}

string IniParser::GetValue(const string& key, const string& section) {
    IniNode* pSection = root_;
    if (section.length() != 0) {
        pSection = root_->Get(section, IniNode::INI_NODE_TYPE_SECTION);
        if (pSection == NULL)   return "";
    }

    IniNode* pKV = pSection->Get(key, IniNode::INI_NODE_TYPE_KEY_VALUE);
    if (pKV == NULL)    return "";
    
    return pKV->value;
}

void   IniParser::SetValue(const string& key, const string& value, const string& section) {
    IniNode* pSection = root_;
    if (section.length() != 0) {
        pSection = root_->Get(section, IniNode::INI_NODE_TYPE_SECTION);
        if (pSection == NULL) {
            pSection = new IniNode;
            pSection->type = IniNode::INI_NODE_TYPE_SECTION;
            pSection->label = section;
            root_->Add(pSection);
        }
    }

    IniNode* pKV = pSection->Get(key, IniNode::INI_NODE_TYPE_KEY_VALUE);
    if (pKV == NULL) {
        pKV = new IniNode;
        pKV->type = IniNode::INI_NODE_TYPE_KEY_VALUE;
        pKV->label = key;
        pSection->Add(pKV);
    }
    pKV->value = value;
}