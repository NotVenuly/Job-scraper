#include <lexbor/html/html.h>
#include <lexbor/css/css.h>
#include <lexbor/selectors/selectors.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <memory>
#include <cstring>


struct SavedJobs{
    std::map<std::string, std::string> jobData;

};

static lxb_status_t Callback(lxb_dom_node_t* node, lxb_css_selector_specificity_t spec, void* voidJob )
{
    SavedJobs* jobs = (SavedJobs*)voidJob;
    lxb_dom_element_t* element = lxb_dom_interface_element(node);
    size_t href_len;
    size_t title_len;

    const lxb_char_t* href = lxb_dom_element_get_attribute(element, (const lxb_char_t*)"href", 4, &href_len);
    const lxb_char_t* title = lxb_dom_node_text_content(node, &title_len);


    if(href && title && href_len > 0 && title_len > 0){
        std::string url((const char*)href, href_len);
        std::string jobName((const char*)title, title_len);
        jobs->jobData[jobName] = url;
    }

    std::cout << "Found job: " << std::string((const char*)title, title_len) << std::endl;

    return LXB_STATUS_OK;
}

std::map<std::string, std::string> Parse(char htmlIN[], size_t htmlIN_len)
{
    SavedJobs voidJobs;
    const lxb_char_t *selector_string = (const lxb_char_t*)"div.job__content.clearfix > h2.node__title.node__title > a";
    size_t selector_length = strlen((const char *)selector_string);

    std::vector<lxb_char_t> html(htmlIN_len + 1, 0);
    for (size_t i = 0; i < htmlIN_len; ++i) {
        html[i] = htmlIN[i];
    }

    lxb_html_document_t *document = lxb_html_document_create();
    if (document == NULL) {
        return std::map<std::string, std::string>();
    }

    lxb_status_t status = lxb_html_document_parse(document, 
        html.data(),
        htmlIN_len);

    if (status != LXB_STATUS_OK) {
        lxb_html_document_destroy(document);
        return std::map<std::string, std::string>();
    }

    lxb_css_parser_t *css_parser = lxb_css_parser_create();
    lxb_css_parser_init(css_parser, NULL);
    lxb_css_selector_list_t *selector_list = lxb_css_selectors_parse(css_parser, selector_string, selector_length);

    lxb_selectors_t *selectors = lxb_selectors_create();
    lxb_selectors_init(selectors);

    status = lxb_selectors_find(
        selectors,
        lxb_dom_interface_node(document),
        selector_list,
        Callback,
        (void*)&voidJobs
    );

    std::cout << "Parsing complete. Found " << voidJobs.jobData.size() << " jobs." << std::endl;

    lxb_selectors_destroy(selectors, true);
    lxb_css_parser_destroy(css_parser, true);
    lxb_css_selector_list_destroy_memory(selector_list);
    lxb_html_document_destroy(document);

    return voidJobs.jobData;
}