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

use proc_macro2::TokenStream as TokenStream2;
use quote::quote;
use syn::{ItemFn, Result};

pub(crate) fn entry(_: TokenStream2, item: TokenStream2) -> Result<TokenStream2> {
    let mut item = syn::parse2::<ItemFn>(item)?;
    let item_clone = item.clone();

    let mut block = quote! {};

    let out = item.sig.output;
    let mut out_arg = None;

    match out {
        syn::ReturnType::Default => {}
        syn::ReturnType::Type(_, output) => match *output {
            syn::Type::Path(path) => {
                let argument = &path.path.segments.iter().next().unwrap().arguments;
                match argument {
                    syn::PathArguments::AngleBracketed(args) => {
                        let arg0 = args.args.iter().next().unwrap();
                        match arg0 {
                            syn::GenericArgument::Type(ty) => match ty {
                                syn::Type::Path(path) => {
                                    let ident =
                                        path.path.segments.iter().next().unwrap().ident.to_string();
                                    out_arg = Some(ident);
                                }
                                syn::Type::Tuple(_) => {}
                                _ => unimplemented!(),
                            },
                            _ => unimplemented!(),
                        }
                    }
                    _ => unimplemented!(),
                }
            }
            _ => unimplemented!(),
        },
    }

    let mut sig = quote! {
        env: ani_rs::AniEnv<'local>,
    };

    let mut input = quote! {};
    for i in item.sig.inputs.iter() {
        if let syn::FnArg::Typed(pat) = i {
            let mut de = Option::<String>::None;
            match &(*pat.ty) {
                syn::Type::Path(p) => {
                    let ident = p.path.segments.iter().next().unwrap().ident.to_string();
                    match ident.as_str() {
                        "i8" | "i16" | "i32" | "i64" | "f32" | "f64" | "bool" | "AniRef" => {
                            de = Some(ident);
                        }
                        _ => {}
                    }
                }
                _ => {}
            };

            if let syn::Pat::Ident(pat) = &*pat.pat {
                if pat.ident.to_string() == "this" {
                    sig = quote! {
                        env: ani_rs::AniEnv<'local>,
                        this: ani_rs::objects::AniObject<'local>,
                    };

                    block = quote! {
                        #block
                        let this = env.deserialize(this).unwrap();
                    };
                    input = quote! {
                        #input
                        this,
                    };
                } else if pat.ident.to_string() == "env" {
                    input = quote! {
                        #input
                        &env,
                    };
                } else if pat.ident.to_string() == "callback" {
                    input = quote! {
                        #input
                        callback,
                    };
                    sig = quote! {
                        #sig
                        #pat: ani_rs::objects::AniFnObject<'local>,
                    }
                } else if pat.ident.to_string() == "async_callback" {
                    input = quote! {
                        #input
                        async_callback,
                    };
                    sig = quote! {
                        #sig
                        #pat: ani_rs::objects::AniAsyncCallback<'local>,
                    }
                } else if pat.ident.to_string() == "error_callback" {
                    input = quote! {
                        #input
                        error_callback,
                    };
                    sig = quote! {
                        #sig
                        #pat: ani_rs::objects::AniErrorCallback<'local>,
                    }
                } else {
                    let pat = pat.ident.clone();
                    if de.is_none() {
                        block = quote! {
                            #block
                            let #pat = env.deserialize(#pat).unwrap();
                        };
                    }
                    input = quote! {
                        #input
                        #pat,
                    };

                    match de {
                        Some(de) => match de.as_str() {
                            "i8" => {
                                sig = quote! {
                                    #sig
                                    #pat: i8,
                                };
                            }
                            "i16" => {
                                sig = quote! {
                                    #sig
                                    #pat: i16,
                                };
                            }
                            "i32" => {
                                sig = quote! {
                                    #sig
                                    #pat: i32,
                                };
                            }
                            "i64" => {
                                sig = quote! {
                                    #sig
                                    #pat: i64,
                                };
                            }
                            "f32" => {
                                sig = quote! {
                                    #sig
                                    #pat: f32,
                                };
                            }
                            "f64" => {
                                sig = quote! {
                                    #sig
                                    #pat: f64,
                                };
                            }
                            "bool" => {
                                sig = quote! {
                                    #sig
                                    #pat: bool,
                                };
                            }
                            "AniRef" => {
                                sig = quote! {
                                    #sig
                                    #pat: ani_rs::objects::AniRef<'local>,
                                };
                            }
                            _ => unimplemented!(),
                        },
                        None => {
                            sig = quote! {
                                #sig
                                #pat: ani_rs::objects::AniObject<'local>,
                            }
                        }
                    }
                }
            }
        }
    }
    let ident = item.sig.ident.clone();

    let block = quote!(
        #item_clone
        #block
        let res = #ident (#input);
    );

    let block = match out_arg {
        Some(out) => match out.as_str() {
            "i8" | "i16" | "i32" | "i64" | "f32" | "f64" | "bool" | "AniRef" => {
                let default = match out.as_str() {
                    "i8" => {
                        sig = quote! {
                            extern "C" fn #ident<'local>(#sig) -> i8
                        };
                        quote! {
                            i8::default()
                        }
                    }
                    "i16" => {
                        sig = quote! {
                            extern "C" fn #ident<'local>(#sig) -> i16
                        };
                        quote! {
                            i16::default()
                        }
                    }
                    "i32" => {
                        sig = quote! {
                            extern "C" fn #ident<'local>(#sig) -> i32
                        };
                        quote! {
                            i32::default()
                        }
                    }
                    "i64" => {
                        sig = quote! {
                            extern "C" fn #ident<'local>(#sig) -> i64
                        };
                        quote! {
                            i64::default()
                        }
                    }
                    "f32" => {
                        sig = quote! {
                            extern "C" fn #ident<'local>(#sig) -> f32
                        };
                        quote! {
                            f32::default()
                        }
                    }
                    "f64" => {
                        sig = quote! {
                            extern "C" fn #ident<'local>(#sig) -> f64
                        };
                        quote! {
                            f64::default()
                        }
                    }
                    "bool" => {
                        sig = quote! {
                            extern "C" fn #ident<'local>(#sig) -> bool
                        };
                        quote! {
                            false
                        }
                    }
                    "AniRef" => {
                        sig = quote! {
                            extern "C" fn #ident<'local>(#sig) -> ani_rs::objects::AniRef<'local>
                        };
                        quote! {
                            env.undefined().unwrap()
                        }
                    }
                    _ => {
                        unimplemented!()
                    }
                };
                quote! {
                    {
                        #block
                        match res {
                            Ok(res) => {
                                res
                            }
                            Err(err) => {
                                let ret = #default;
                                env.throw_business_error(err.code(), err.message())
                                    .unwrap();
                                ret
                            }
                        }
                    }
                }
            }
            _ => {
                sig = quote! {
                    extern "C" fn #ident<'local>(#sig) -> ani_rs::objects::AniRef<'local>
                };
                quote! {
                {
                    #block
                    match res {
                        Ok(res) => {
                            env.serialize(&res).unwrap()
                        }
                        Err(err) => {
                            let res = env.undefined().unwrap();
                            env.throw_business_error(err.code(), err.message())
                                .unwrap();
                            res
                        }
                    }
                    }
                }
            }
        },
        None => {
            sig = quote! {extern "C" fn #ident<'local>(#sig)};
            quote! {
                {
                    #block
                    if let Err(err) =  res {
                        env.throw_business_error(err.code(), err.message())
                            .unwrap();
                    }
                }
            }
        }
    };

    let sig = syn::parse2(sig).unwrap();
    item.sig = sig;

    item.block = syn::parse2(block).unwrap();

    Ok(quote! {
        #item
    })
}
