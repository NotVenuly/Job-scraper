#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <cstring>
#include <algorithm>
#include <cctype>

#include <cpr/cpr.h>
#include <lexbor/html/html.h>
#include <lexbor/css/css.h>
#include <lexbor/selectors/selectors.h>


struct SavedJobs{
    std::map<std::string, std::pair<std::string, std::string> > jobData;

};

void ParseChildren(lxb_dom_node_t* parent, std::string& out);
void ParseNode(lxb_dom_node_t* node, std::string& out);
std::map<std::string, std::pair<std::string, std::string>> Parse(char htmlIN[], size_t htmlIN_len, const char selectorStr[], std::string* returnedStr, bool isDetail);

std::string Trimmer(std::string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));

    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());

    return s;
}

void ParseChildren(lxb_dom_node_t* parent, std::string& out)
{
    for (lxb_dom_node_t* child = parent->first_child; child != nullptr; child = child->next)
    {
        ParseNode(child, out);
    }
}

void ParseNode(lxb_dom_node_t* node, std::string& out)
{
    if (node == nullptr)
        return;

    if (node->type == LXB_DOM_NODE_TYPE_TEXT) {
        size_t len;
        const lxb_char_t* text = lxb_dom_node_text_content(node, &len);

        if (text && len > 0) {
            out += Trimmer(std::string((const char*)text, len));
        }

        return;
    }

    lxb_dom_element_t* element = lxb_dom_interface_element(node);
    lxb_tag_id_t tag = lxb_dom_element_tag_id(element);

    switch (tag) {
        case LXB_TAG_P:
            ParseChildren(node, out);
            out += "\n\n";
            break;

        case LXB_TAG_LI:
            out += "* ";
            ParseChildren(node, out);
            out += "\n";
            break;

        case LXB_TAG_H2:
            out += "## ";
            ParseChildren(node, out);
            out += "\n\n";
            break;

        case LXB_TAG_H3:
            out += "### ";
            ParseChildren(node, out);
            out += "\n\n";
            break;

        default:
            ParseChildren(node, out);
    }
}

static lxb_status_t Detail(lxb_dom_node_t* node, lxb_css_selector_specificity_t spec, void* voidText)
{
    std::string* textPtr = (std::string*)voidText;

    ParseNode(node, *textPtr);

    return LXB_STATUS_OK;
}
    
static lxb_status_t Callback(lxb_dom_node_t* node, lxb_css_selector_specificity_t spec, void* voidJob )
{
    SavedJobs* jobs = (SavedJobs*)voidJob;
    lxb_dom_element_t* element = lxb_dom_interface_element(node);
    size_t href_len;
    size_t title_len;
    
    const lxb_char_t* href = lxb_dom_element_get_attribute(element, (const lxb_char_t*)"href", 4, &href_len);
    const lxb_char_t* title = lxb_dom_node_text_content(node, &title_len);
    
    
    if(href && title && href_len && title_len> 0){
        
        std::string url((const char*)href, href_len);
        std::string jobName((const char*)title, title_len);
        std::string returnedStr;
        
        try{
            cpr::Response r = cpr::Get(cpr::Url{url});
            
            size_t listHtml_len = r.text.length();
            std::vector<char> listHtml(listHtml_len + 1, 0);
            
            for (size_t i = 0; i < listHtml_len; ++i) {
                listHtml[i] = r.text[i];
            }
            
            listHtml[listHtml_len] = '\0';
            
            Parse(listHtml.data(), listHtml_len, "div.field__item.even", &returnedStr, true);


        }
        catch (const std::exception& e) {
            std::cout << "Exception: " << e.what() << std::endl;
            std::cout << "Press Enter to exit..." << std::endl;
            std::cin.get();
        }
        
        jobs->jobData[jobName] = std::make_pair(url, returnedStr);
    }
    
    std::cout << "Found job: " << std::string((const char*)title, title_len) << std::endl;
    
    return LXB_STATUS_OK;
}

static lxb_status_t CallFind(lxb_selectors* selectors, lxb_html_document* document, lxb_css_selector_list* selector_list, std::string* usedStr, SavedJobs* jobClass, bool isDetail){

    if(isDetail){

        return lxb_selectors_find(
            selectors,
            lxb_dom_interface_node(document),
            selector_list,
            Detail,
            (void*)usedStr
        );
    }else{

        return lxb_selectors_find(
            selectors,
            lxb_dom_interface_node(document),
            selector_list,
            Callback,
            (void*)jobClass
        );
    }
}

std::map<std::string, std::pair<std::string, std::string>> Parse(char htmlIN[], size_t htmlIN_len, const char selectorStr[], std::string* returnedStr, bool isDetail)
{
    SavedJobs jobClass;
    const lxb_char_t *selector_string = (const lxb_char_t*)selectorStr;
    size_t selector_length = strlen((const char *)selector_string);
    
    std::vector<lxb_char_t> html(htmlIN_len + 1, 0);
    for (size_t i = 0; i < htmlIN_len; ++i) {
        html[i] = htmlIN[i];
    }

    lxb_html_document_t *document = lxb_html_document_create();
    if (document == NULL) {
        return std::map<std::string, std::pair<std::string, std::string>>();
    }
    
    lxb_status_t status = lxb_html_document_parse(document, 
        html.data(),
        htmlIN_len);
        
        if (status != LXB_STATUS_OK) {
            lxb_html_document_destroy(document);
            return std::map<std::string, std::pair<std::string, std::string>>();
        }

    lxb_css_parser_t *css_parser = lxb_css_parser_create();
    lxb_css_parser_init(css_parser, NULL);
    lxb_css_selector_list_t *selector_list = lxb_css_selectors_parse(css_parser, selector_string, selector_length);

    lxb_selectors_t *selectors = lxb_selectors_create();
    lxb_selectors_init(selectors);

    status = CallFind(
        selectors,
        document,
        selector_list,
        returnedStr,
        &jobClass,
        isDetail
    );

    std::cout << "Parsing complete." << std::endl;
    std::cout << "-------------------------------------------------------------------------------------" << std::endl;

    lxb_selectors_destroy(selectors, true);
    lxb_css_parser_destroy(css_parser, true);
    lxb_css_selector_list_destroy_memory(selector_list);
    lxb_html_document_destroy(document);

    return jobClass.jobData;
}