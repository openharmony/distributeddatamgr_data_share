// Copyright (C) 2025 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

extern crate proc_macro;

mod native;

use proc_macro::TokenStream;
use proc_macro2::TokenStream as TokenStream2;
use quote::{quote, ToTokens};
use syn::{
    parse::{Parse, ParseStream, Parser},
    punctuated::Punctuated,
    spanned::Spanned,
    token::Comma,
    Attribute, Error, ItemEnum, ItemStruct, LitStr, MetaNameValue, Result,
};

#[proc_macro_attribute]
pub fn ani(args: TokenStream, item: TokenStream) -> TokenStream {
    entry(TokenStream2::from(args), TokenStream2::from(item))
        .unwrap()
        .into()
}

#[proc_macro_attribute]
pub fn native(args: TokenStream, item: TokenStream) -> TokenStream {
    native::entry(TokenStream2::from(args), TokenStream2::from(item))
        .unwrap()
        .into()
}

fn entry(args: TokenStream2, item: TokenStream2) -> Result<TokenStream2> {
    let args = Punctuated::<MetaNameValue, Comma>::parse_terminated.parse2(args)?;

    let mut config = None;

    let mut output_only = false;
    for arg in args {
        let ident = arg
            .path
            .get_ident()
            .ok_or(Error::new(arg.path.span(), "Invalid ident"))?;

        let ident_string = ident.to_string();

        match ident_string.as_str() {
            "path" => config = Some(arg.value),
            "output" => {
                let s = arg.value.to_token_stream();
                let s = syn::parse2::<LitStr>(s)
                    .map_err(|item| Error::new(item.span(), "Invalid Attribute `path`"))?;
                if s.value() == "only" {
                    output_only = true;
                } else {
                    return Err(Error::new(item.span(), "Invalid `output`"));
                }
            }
            name => {
                return Err(Error::new(
                    arg.path.span(),
                    format!("Invalid Attribute `{name}`",),
                ))
            }
        }
    }
    let mut rename = String::new();
    if let Some(config) = config.as_mut() {
        let s = config.to_token_stream();
        let s = syn::parse2::<LitStr>(s)
            .map_err(|item| Error::new(item.span(), "Invalid Attribute `path`"))?;
        rename = s.value();
        rename.push('\0');
    }
    if let Ok(mut item) = syn::parse2::<ItemStruct>(item.clone()) {
        for field in &mut item.fields {
            let ident = field.ident.as_ref().unwrap();
            let ident = ident.to_string();
            let mut camel = String::new();
            let mut capitalize = false;
            for ch in ident.chars() {
                if ch == '_' {
                    capitalize = true;
                } else if capitalize {
                    camel.push(ch.to_ascii_uppercase());
                    capitalize = false;
                } else {
                    camel.push(ch);
                }
            }
            camel.push('\0');
            let attr = quote! {#[serde(rename = #camel)]};
            let attr = syn::parse2::<Attr>(attr)
                .map_err(|item| Error::new(item.span(), "Invalid Attribute"))?;
            field.attrs.push(attr.attr);
        }
        if rename.is_empty() {
            Ok(quote! {
                #[derive(serde::Deserialize)]
                #item
            })
        } else {
            if output_only {
                Ok(quote! {
                    #[derive(serde::Serialize)]
                    #[serde(rename = #rename)]
                    #item
                })
            } else {
                Ok(quote! {
                    #[derive(serde::Serialize, serde::Deserialize)]
                    #[serde(rename = #rename)]
                    #item

                })
            }
        }
    } else {
        let mut item = syn::parse2::<ItemEnum>(item).unwrap();
        for variant in &mut item.variants {
            let ident = variant.ident.to_string();
            let mut snake = String::new();
            for (i, ch) in ident.char_indices() {
                if i > 0 && ch.is_uppercase() {
                    snake.push('_');
                }
                snake.push(ch.to_ascii_lowercase());
            }
            let mut snake = snake.to_uppercase();
            snake.push('\0');
            let attr = quote! {#[serde(rename = #snake)]};
            let attr = syn::parse2::<Attr>(attr)
                .map_err(|item| Error::new(item.span(), "Invalid Attribute"))?;
            variant.attrs.push(attr.attr);
        }
        Ok(quote! {
            #[derive(serde::Serialize, serde::Deserialize)]
            #[serde(rename = #rename)]
            #item

        })
    }
}

struct Attr {
    attr: Attribute,
}

impl Parse for Attr {
    fn parse(input: ParseStream) -> Result<Self> {
        let attr = input.call(Attribute::parse_outer)?.pop().unwrap();
        Ok(Attr { attr })
    }
}
