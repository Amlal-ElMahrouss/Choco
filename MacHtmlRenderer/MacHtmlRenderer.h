//
//  MacHtmlRenderer.h
//  test
//
//  Created by Amlal on 10/19/24.
//

#ifndef MacHtmlRenderer_h
#define MacHtmlRenderer_h

#import <Cocoa/Cocoa.h>
#include <vector>
#include <string>

class IZkDOM {
public:
    explicit IZkDOM() = default;
    virtual ~IZkDOM() = default;
    
    IZkDOM& operator=(const IZkDOM&) = default;
    IZkDOM(const IZkDOM&) = default;
    
    virtual bool insert_element(NSWindow* window) = 0;
    virtual bool remove_element() = 0;
    
    virtual bool insert_child_element(IZkDOM* dom_elem) = 0;
    virtual bool remove_child_element(IZkDOM* dom_elem) = 0;
    
public:
    void set_content_text(NSString* text) {
        h_markup_content = text;
    }
    
    void set_position(CGFloat x, CGFloat y) {
        h_x = x;
        h_y = y;
    }
    
public:
    enum {
        kHeadingInvalid,
        kHeading1 = 100,
        kHeading2,
        kHeading3,
        kHeading4,
        kHeading5,
        kHeading6,
        kHeadingParagraph,
    };
    
    void set_heading(int32_t text) {
        switch (text) {
            case kHeading1:
                h_font_sz = 32.0;
                break;
            case kHeading2:
                h_font_sz = 24.0;
                break;
            case kHeading3:
                h_font_sz = 17.8;
                break;
            case kHeading4:
            case kHeading5:
                h_font_sz = 13.28;
                break;
            case kHeading6:
                h_font_sz = 12;
                break;
            case kHeadingParagraph:
                h_font_sz = 16;
                break;
            default:
                break;
        }
    }
    
    virtual bool is_text_node() { return false; }
    
protected:
    std::vector<IZkDOM*> h_child_element;
    bool h_renderable{true};
    CGFloat h_x, h_y{0};
    
protected:
    NSString* h_font{@"SF Compact Rounded"};
    NSString* h_font_bold{@"SF Compact Rounded Bold"};
    NSString* h_markup_content{@""};
    CGFloat h_font_sz{16.0f};
    
};

class ZkTextDOM : public IZkDOM {
protected:
    NSTextView *h_content{nullptr};
    
public:
    explicit ZkTextDOM() = default;
    virtual ~ZkTextDOM() = default;
    
    ZkTextDOM& operator=(const ZkTextDOM&) = default;
    ZkTextDOM(const ZkTextDOM&) = default;
    
public:
    bool insert_child_element(IZkDOM* dom_elem) override {
        if (!dom_elem)
            return false;
        
        this->h_child_element.push_back(dom_elem);
        return true;
    }
    
    bool is_text_node() override { return true; }
    
    bool remove_child_element(IZkDOM* dom_elem) override {
        if (!dom_elem)
            return false;
        
        auto elem = std::find(this->h_child_element.begin(), this->h_child_element.end(), dom_elem);
        
        if (elem != this->h_child_element.end())
        {
            this->h_child_element.erase(elem);
            return true;
        }
        
        return false;
    }
    
    bool remove_element() override {
        if (h_content) {
            [h_content removeFromSuperview];
            
            for (auto& elem : h_child_element) {
                elem->remove_element();
            }
            
            return true;
        }
        
        return false;
    }
    
    bool insert_element(NSWindow* window) override {
        if (!h_renderable || !window ||
            h_content)
            return false;
        
        this->h_y += [[window contentView] frame].origin.y + 1;
        this->h_x += [[window contentView] frame].origin.x + 1;
        
        h_content = [[NSTextView alloc] initWithFrame:CGRectMake(-this->h_x, -this->h_y, [[window contentView] frame].size.width, [[window contentView] frame].size.height)];

        [h_content setTextColor:[NSColor blackColor]];
        [h_content setBackgroundColor:[NSColor clearColor]];
        [h_content setString:h_markup_content];
        
        if (h_font_sz <= 13.28)
        {
            [h_content setFont:[NSFont fontWithName: this->h_font_bold size: this->h_font_sz]];
        }
        else
        {
            [h_content setFont:[NSFont fontWithName: this->h_font size: this->h_font_sz]];
        }
        
        [[window contentView] addSubview:h_content];
        
        for (auto& elem : h_child_element) {
            elem->set_position(this->h_x, this->h_y + this->h_font_sz);
            
            elem->insert_element(window);
        }
        
        return true;
    }
};


class ZkImageDOM : public ZkTextDOM {
protected:
    NSTextView *h_placeholder_content{nullptr};
    NSURL* h_image_path{nullptr};
    NSImage *h_image_content{nullptr};
    NSImageView* h_image_view{nullptr};
    
public:
    explicit ZkImageDOM() = default;
    virtual ~ZkImageDOM() = default;
    
    ZkImageDOM& operator=(const ZkImageDOM&) = default;
    ZkImageDOM(const ZkImageDOM&) = default;
    
    bool set_image_url(NSURL* url_img) {
        if (!url_img)
            return false;
        
        h_image_path = url_img;
        return true;
    }
    
    bool insert_child_element(IZkDOM* dom_elem) override {
        if (!dom_elem)
            return false;
        
        this->h_child_element.push_back(dom_elem);
        return true;
    }
    
    bool is_text_node() override { return false; }
    
    bool remove_child_element(IZkDOM* dom_elem) override {
        if (!dom_elem)
            return false;
        
        auto elem = std::find(this->h_child_element.begin(), this->h_child_element.end(), dom_elem);
        
        if (elem != this->h_child_element.end())
        {
            this->h_child_element.erase(elem);
            return true;
        }
        
        return false;
    }
    
    bool remove_element() override {
        if (h_content) {
            [h_content removeFromSuperview];
            
            for (auto& elem : h_child_element) {
                elem->remove_element();
            }
            
            return true;
        }
        
        return false;
    }
    
    bool insert_element(NSWindow* window) override {
        if (!h_renderable || !window ||
            h_content)
            return false;
        
        this->h_y += [[window contentView] frame].origin.y + 1;
        this->h_x += [[window contentView] frame].origin.x + 1;
        
        h_image_content = [[NSImage alloc] initWithContentsOfURL:h_image_path];
        
        if (!h_image_content)
        {
            h_placeholder_content = [[NSTextView alloc] initWithFrame:CGRectMake(-this->h_x, -this->h_y, [[window contentView] frame].size.width, [[window contentView] frame].size.height)];

            [h_placeholder_content setTextColor:[NSColor blackColor]];
            [h_placeholder_content setBackgroundColor:[NSColor clearColor]];
            [h_placeholder_content setString:h_markup_content];
            
            if (h_font_sz <= 13.28)
            {
                [h_placeholder_content setFont:[NSFont fontWithName: this->h_font_bold size: this->h_font_sz]];
            }
            else
            {
                [h_placeholder_content setFont:[NSFont fontWithName: this->h_font size: this->h_font_sz]];
            }
            
            [[window contentView] addSubview:h_placeholder_content];
        }
        else
        {
            h_image_view = [[NSImageView alloc] initWithFrame:CGRectMake(-this->h_x, -this->h_y, [h_image_content size].width, [h_image_content size].height)];
            
            [h_image_view setImage:h_image_content];
            
            [[window contentView] addSubview:h_image_view];
        }
        
        for (auto& elem : h_child_element) {
            elem->set_position(this->h_x, this->h_y + [h_image_content size].height);
            elem->insert_element(window);
        }
        
        return true;
    }
};

#endif /* MacHtmlRenderer_h */
