//
//  NSString+HTMLTags.m
//  Links
//
//  Created by Sergey Plotnikov on 23/07/14.
//  Copyright (c) 2014 Sergey Plotnikov. All rights reserved.
//

#import "NSString+HTMLTags.h"

@implementation NSString (HTMLTags)


-(NSString *)getHREFValue{
    
    NSError *error = NULL;
    
    NSString *result;
    
    NSRegularExpression *regexp = [NSRegularExpression regularExpressionWithPattern:@"(?<=href=\")(.*?)(?=\")" options:NSRegularExpressionCaseInsensitive error:&error];
    
    NSRange rangeOfFirstMatch = [regexp rangeOfFirstMatchInString:self options:0 range:NSMakeRange(0, [self length])];
    
    if (rangeOfFirstMatch.location != NSNotFound) {
        
        result = [self substringWithRange:rangeOfFirstMatch];
 
        
    }
    
    return result;
}



-(NSString *)searchForFaviconURL{
    
    NSString *result;
    
    
    //create NSRegular expression instance
    NSError *error = NULL;
    
    //first version
    /*NSRegularExpression *regex = [NSRegularExpression
                                  regularExpressionWithPattern:@"<.*(rel=\"shortcut icon\"|rel=\"icon\").*>"
                                  options:NSRegularExpressionCaseInsensitive
                                  error:&error];
     
     */
    
    //a regular expression to get content between rel="shortcut icon" or rel="icon" and >
    
    NSRegularExpression *regex = [NSRegularExpression
                                  regularExpressionWithPattern:@"(?<=(rel=\"shortcut icon\"|rel=\"icon\"))(.*?)(?=>)"
                                  options:NSRegularExpressionCaseInsensitive
                                  error:&error];

    
    
    //get first occurence
    NSRange rangeOfFirstMatch = [regex rangeOfFirstMatchInString:self options:0 range:NSMakeRange(0, [self length])];
    
    
    
    if (rangeOfFirstMatch.location != NSNotFound) {
        
        NSString *faviconString = [self substringWithRange:rangeOfFirstMatch];
        
        
        //check for "href"
        
        BOOL isHREF = [faviconString containsString:@"href"];
        
        if (isHREF) {
            
            result = [faviconString getHREFValue];
        }
        
        else{
            //check between < and rel="shortcut icon" or rel="icon"
            
            NSRegularExpression *regex = [NSRegularExpression
                                          regularExpressionWithPattern:@"(?<=<)(.*?)(?=(rel=\"shortcut icon\"|rel=\"icon\"))"
                                          options:NSRegularExpressionCaseInsensitive
                                          error:&error];
            
            //get first occurence
            NSRange rangeOfFirstMatch = [regex rangeOfFirstMatchInString:self options:0 range:NSMakeRange(0, [self length])];
            
            if (rangeOfFirstMatch.location != NSNotFound) {
                
                NSString *faviconString = [self substringWithRange:rangeOfFirstMatch];
                
                
                //check for "href"
                
                BOOL isHREF = [faviconString containsString:@"href"];
                
                if (isHREF) {
                    
                    result = [faviconString getHREFValue];
                }
            
            }
        }
        
        
    }
    
    //replace http for https
    
    //result = [result stringByReplacingOccurrencesOfString:@"http" withString:@"https"];
    
    return result;
    
}


-(BOOL)containsString:(NSString *)string{
    
    NSRange range = [self rangeOfString:string];
    
    if (range.location == NSNotFound) {
        return FALSE;
    }
    else {
        
        return TRUE;
        
    }
}


-(NSString *)searchForValueForTag:(NSString *)tag{
    
    NSString *result;
    
    NSString *tagEnd = [NSString stringWithFormat:@"</%@>",tag];
    
    
    NSRange titleEnd = [self rangeOfString:tagEnd];
    
    if (titleEnd.location != NSNotFound) {
        
        
        //regular expression to cover possible parameters(<title id="pageTitle"> on facebook.com for example)
        
        NSString *tagBegin = [NSString stringWithFormat:@"<%@.*?>",tag];
        
        NSError *error = NULL;
        
        NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:tagBegin options:NSRegularExpressionCaseInsensitive error:&error];
        
        

        
        //get range with regular expression
        NSRange titleBegin = [regex rangeOfFirstMatchInString:self options:0 range:NSMakeRange(0, self.length)];
        
        
        NSRange titleRange = NSMakeRange(titleBegin.location + titleBegin.length, titleEnd.location - titleBegin.location - titleBegin.length);
        
        if (titleRange.location != NSNotFound) {
            
            result = [self substringWithRange:titleRange];
        }
        
        
        
        
        
    }

    return result;
    
    
    
}

@end
