// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 2026 Drevton
#ifndef PT_TRADE_H
#define PT_TRADE_H
#include "state_types.h"
#include "layout.h"

// Stock menu.

void DoContractThingy(Supplier *supplier, const i16 *amounts, Rest *restaurant);
u8 AskHowToJustifyDemands(void);
Contract *HandleNegotiateContract(Contract *contract, Supplier *supplier, Rest *restaurant, const i16 *amounts);
void InitializeContract(Contract *contract, const Supplier *supplier, const Rest *restaurant, const i16 *amounts,
    u8 discount, i32 penalty, u16 charge, u8 term);
PTCanvasResult DrawContractTerms(const Contract *contract);
u32 GetSupplierContractPhoneAlternatives(u8 *array, u8 count, u8 base);
void DrawContractDetails(const Contract *contract, u8 page);
void ContractCancel(Contract *contract);
void HandleContractsMenu(Rest *restaurant);
i32 GetSupplierQuality(u8 packed_quality);
u32 DrawSupplierBitmap(u8 quality, u8 row);
u16 DrawSupplierName(const u8 *name, i16 x, i16 y);
void ClearDepotPurchases(void);
u8 *CutoffString(const u8 *source, u8 maxlen);
void NotifyWarehouseFull(void);
void DrawDepotPurchaseCaptions(const Depot *depot, const Supplier *supplier, u8 mode);
void DrawDepotPurchaseStuff(const Supplier *supplier, const Depot *depot, i8 index, u8 color, u8 selling);
void HandleBuyStockToDepot(u8 player, u8 mode, Supplier *supplier);
void HandleDepotExport(i8 country, u8 stock_kind);
void DepotDragStockBg(u8 kind);
void DrawDepotContents(const Depot *depot, u8 kind, u8 town);
u8 GetDepotExportCountry(void);
u8 HandleDepotPriorities(Player *player);
u8 HandleCentralDepot(u8 player);
void HandleDepotWeapons(Poi *poi);
void HandleDepotWeapons_DrawElem(Poi *poi, i8 row, u8 selected);
void DrawShopContents(const Supplier *supplier, const Rest *restaurant, i8 index, u8 color);
void UpdateTotalPurchasePrice(u8 count);
u16 HandleStockShopMenu(Rest *restaurant, u8 stock_kind, u8 mode);
void HandleBuyStockToRest(Supplier *supplier, Rest *restaurant);
u8 HandleStockMenu(Rest *restaurant);

// Supplier day.

void SupplierDecreaseItemsLeft(Supplier *supplier, u8 ingredient, u16 decrease);
void RefillSupplierItems(u8 stock_kind);
void RunSuppliersDailyLoop(void);
void SupplierReduceBoughtByPlayer(void);

#endif
