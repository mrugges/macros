|------------------------------------------------------------------------|
|- e3_Macro_Sell.inc v1.5 ( originally designed to work with e3.mac )	-|
|- Written by Killians of PEQ											-|
|------------------------------------------------------------------------|
|- Companion file for e3_Macro_Loot.inc.								-|
|- Sells items flagged with [/Sell] in your Loot_Ini.					-|
|------------------------------------------------------------------------|                                              -|
|- Several helper functions are available for individual items on your  -|
|- 	/printitemstatus - prints the Loot Settings value for the item on   -|
|-        the cursor.                                                   -|
|- 	/sellthisitem - updates the Loot Settings value for the item on     -|
|-        the cursor to Keep,Sell so that when you invoke /autosell in  -|
|-        future, the item will get sold to the vendor.                 -|
|- 	/keepthisitem - updates the Loot Setings value for the item on      -|
|-        the cursor to Keep so that when /autosell is invoked in       -|
|-        the future, the item will NOT be sold                         -|
|- 	/destroythisitem - updates the Loot Settings value for the item on  -|
|-        the cursor to Destroy so that these items will be destroyed   -|
|-        when /autosell is invoked in the future.                      -|
|------------------------------------------------------------------------|

|----------------------------------------------------|
|- syncInventory									-|
|----------------------------------------------------|
|- Adds items from you inventory to your Loot_Ini.	-|
|----------------------------------------------------|
#EVENT syncInventory "<#*#> Inventory Items"
#EVENT syncInventory "[MQ2] Inventory Items"
#EVENT syncInventory "<#*#> Update Loot.ini"
#EVENT syncInventory "[MQ2] Update Loot.ini"
#EVENT syncInventory "<#*#> Sync Inventory"
#EVENT syncInventory "[MQ2] Sync Inventory"
#EVENT syncInventory "<#*#> Sync Loot.ini"
#EVENT syncInventory "[MQ2] Sync Loot.ini"
SUB EVENT_syncInventory
/if (${Debug} || ${Debug_Sell}) /echo |- event_inventoryItems ==>
	
	/echo Syncing inventory with ${Loot_Ini}...

	/declare i int local
	/declare x int local

	| Check inventory slots 1 - ${numInventorySlots} for items, and containers.
	/for i 1 to ${numInventorySlots}
	
		| If there is an item in inventory slot ${i}
		/if (${Bool[${InvSlot[pack${i}].Item}]}) {
			/call get_lootSetting "pack${i}" 0
		
			| If the item is a container..
			/if (${InvSlot[pack${i}].Item.Container}) {
			
				/for x 1 to ${InvSlot[pack${i}].Item.Container}
			
					/if (${Bool[${Me.Inventory[pack${i}].Item[${x}]}]}) /call get_lootSetting "pack${i}" "${x}"
					
				/next x
			}
		}
	
	/next i
/if (${Debug} || ${Debug_Sell}) /echo <== event_inventoryItems -|
/RETURN

|------------------------------------------------------------------------------------------------------------------------------------|
|- EVENT_sellItems																													-|
|------------------------------------------------------------------------------------------------------------------------------------|
|- Commands bots, including yourself, to open trade with the nearest merchant, and sell items flagged as [/Sell] in your Loot_Ini.	-|
|------------------------------------------------------------------------------------------------------------------------------------|
#EVENT sellItems "[MQ2] Sell Items"
#EVENT sellItems "<#*#> Sell Items"
#EVENT sellItems "[MQ2] AutoSell"
#EVENT sellItems "<#*#> AutoSell"
#EVENT sellItems "[MQ2] Auto Sell"
#EVENT sellItems "<#*#> Auto Sell"
SUB EVENT_sellItems
/if (${Debug} || ${Debug_Sell}) /echo |- EVENT_sellItems ==>
	
	| Get current autoFlagSoldItems value so we can disable and revert if needed.
	/declare currentAutoFlagSoldItems bool local ${autoFlagSoldItems}
	
	| Record starting location.
	/declare startingLoc string local ${Me.Loc.Replace[ ,]}
		
	| Sync inventory with Loot_Ini.
	| /call EVENT_syncInventory
		
	| Open trade with the nearest merchant.
	/call openMerchant
		
	/if (${Window[MerchantWnd].Open}) {
	
		| Open bags.
		/keypress OPEN_INV_BAGS
	
		| Sell Items.
		/call sellItems
		
		| Move back to starting location.	
		/squelch /moveto loc ${startingLoc.Arg[1,,]} ${startingLoc.Arg[2,,]}
		
		| Close merchant.
		/if (${Window[MerchantWnd].Open}) /call closeMerchant
	
		| Destroy bad items.
		/if (${DestroyableItems.Size}) /call destroyItems
	
		| Close bags.
		/keypress CLOSE_INV_BAGS
		
		/echo I have finished selling with autosell.
	}
/if (${Debug} || ${Debug_Sell}) /echo <== EVENT_sellItems -|
/RETURN

|----------------------------------------------------|
|- printitemstatus
|----------------------------------------------------|
|- echo the Loot Setting value for the item on the cursor
|----------------------------------------------------|
#EVENT printItemStatus "/bc printitemstatus"
#EVENT printItemStatus "<#*#> printitemstatus"
SUB EVENT_printItemStatus(line)
/if (${Debug} || ${Debug_Sell}) /echo |- EVENT_printeItemStatus ==>
	/declare cursorItem string local
	/declare cursorIniEntry string local
	/declare cursorIniName string local
	
	/if (${Cursor.ID}) {
		/call getIniKey "${Cursor.Name}"
		/varset cursorIniEntry ${Macro.Return}
		/echo cursorIniEntry = ${cursorIniEntry}
		/varset cursorIniName ${Ini[${Loot_Ini},${cursorIniEntry}]}
		/if (${Defined[cursorIniName]}) {
			/echo ${Cursor.Name} - ${cursorIniName}
		} else {
			/echo No entry found for ${Cursor.Name}
		}
	}
/if (${Debug} || ${Debug_Sell}) /echo <== EVENT_printeItemStatus-|
/RETURN

|----------------------------------------------------|
|- sellThisItem
|----------------------------------------------------|
|- When an item is on the cursor and you call this
|- function it will set the Loot Settings.ini to Keep,Sell
|----------------------------------------------------|
#EVENT sellThisItem "/bc sellthisitem"
#EVENT sellThisItem "<#*#> sellthisitem"
SUB EVENT_sellThisItem(line)
/if (${Debug}) /echo |- EVENT_sellThisItem ==>
	/declare cursorItem string local
	/declare cursorIniEntry string local
	
	/if (${Cursor.ID}) {
		/call getIniKey "${Cursor.Name}"
		/varset cursorIniEntry ${Macro.Return}
		/echo Flagging ${Cursor.Name} in Loot Settings.ini for Keep,Sell
		/call WriteToIni "${Loot_Ini},${cursorIniEntry}" "Keep,Sell" TRUE
	}
/if (${Debug}) /echo <== EVENT_sellThisItem-|
/RETURN

|----------------------------------------------------|
|- destroyThisItem
|----------------------------------------------------|
|- When an item is on the cursor and you call this
|- function it will set the Loot Settings.ini to Destroy
|----------------------------------------------------|
#EVENT destroyThisItem "/bc destroythisitem"
#EVENT destroyThisItem "<#*#> destroythisitem"
SUB EVENT_destroyThisItem(line)
/if (${Debug}) /echo |- EVENT_destroyThisItem ==>
	/declare cursorItem string local
	/declare cursorIniEntry string local
	
	/if (${Cursor.ID}) {
		/call getIniKey "${Cursor.Name}"
		/varset cursorIniEntry ${Macro.Return}
		/echo Flagging ${Cursor.Name} in Loot Settings.ini for Destroy
		/call WriteToIni "${Loot_Ini},${cursorIniEntry}" "Destroy" TRUE
		/destroy
	}
/if (${Debug}) /echo <== EVENT_destroyThisItem-|
/RETURN

|----------------------------------------------------|
|- keepThisItem
|----------------------------------------------------|
|- When an item is on the cursor and you call this
|- function it will set the Loot Settings.ini to Keep
|----------------------------------------------------|
#EVENT keepThisItem "/bc keepthisitem"
#EVENT keepThisItem "<#*#> keepthisitem"
SUB EVENT_keepThisItem(line)
/if (${Debug}) /echo |- EVENT_keepThisItem ==>
	/declare cursorItem string local
	/declare cursorIniEntry string local
	
	/if (${Cursor.ID}) {
		/call getIniKey "${Cursor.Name}"
		/varset cursorIniEntry ${Macro.Return}
		/echo Flagging ${Cursor.Name} in Loot Settings.ini for Keep
		/echo ${cursorIniEntry}
		/call WriteToIni "${Loot_Ini},${cursorIniEntry}" "Keep" TRUE
	}
/if (${Debug}) /echo <== EVENT_keepThisItem-|
/RETURN

|----------------------------------------|
|- Opens all containers in inventory.	-|
|----------------------------------------|
SUB openBags
/if (${Debug} || ${Debug_Sell}) /echo |- openBags ==>

	| Open all bags in inventory.
	/declare i int local
	/declare retryTimer timer local
	/declare miscTimer timer local
	
	/for i 1 to ${numInventorySlots}
		
		/varset miscTimer 30
		/if (${Me.Inventory[pack${i}].Container}) {
			
			:openBag
			/if (${Debug} || ${Debug_Sell}) /echo |- openBags -| :openBag
		
			| Open container
			/if (!${Window[pack${i}].Open}) {
				/itemnotify pack${i} rightmouseup
				/delay 5 ${Window[pack${i}].Open} || ${Window[TradeSkillWnd].Open}
				/if (!${Window[pack${i}].Open} && !${Window[TradeSkillWnd].Open} && ${miscTimer}) /goto :openBag
				
				| Open tradeskill container.
				/if (${Window[TradeSkillWnd].Open}) {
					:openExperimentWnd
					/notify TradeSkillWnd ExperimentButton leftmouseup
					/delay 5 !${Window[TradeSkillWnd].Open}
					/if (${Window[TradeSkillWnd].Open} && ${miscTimer}) /goto :openExperimentWnd
				}
			}
		}
	
	/next i
/if (${Debug} || ${Debug_Sell}) /echo <== openBags -|
/RETURN

|----------------------------------------|
|- Closes all containers in inventory.	-|
|----------------------------------------|
SUB closeBags
/if (${Debug} || ${Debug_Sell}) /echo |- closeBags ==>

	| Close all bags in inventory.
	/declare i int local
	/declare retryTimer timer local
	/declare miscTimer timer local
	
	/for i 1 to ${numInventorySlots}
		
		/varset miscTimer 30
		/if (${Me.Inventory[pack${i}].Container}) {
			
		:closeBag
		/if (${Debug} || ${Debug_Sell}) /echo |- closeBags -| :closeBag
		
			| Open container
			/if (${Window[pack${i}].Open}) {
				/itemnotify pack${i} rightmouseup
				/delay 5 !${Window[pack${i}].Open}
				/if (${Window[pack${i}].Open} && ${miscTimer}) /goto :closeBag
			}
		}

	/next i
	
/if (${Debug} || ${Debug_Sell}) /echo <== closeBags -|
/RETURN

|--------------------------------------------|
|- Opens trade with the nearest merchant.	-|
|--------------------------------------------|
SUB openMerchant(int providedID)
/if (${Debug} || ${Debug_Sell}) /echo |- openMerchant ==>

	/declare miscTimer timer local
	/declare merchantTotal int local -1
	/declare merchantID int local ${If[${providedID},${providedID},${Spawn[Merchant].ID}]}

	/if (!${Spawn[Merchant].ID} || !${Spawn[Merchant].LineOfSight}) {
		/echo There are no merchants nearby!
	} else {
	
		| Move to the merchant.
		/call TrueTarget ${merchantID}
		/call MoveToLoc ${Target.Y} ${Target.X} 80 15
	
		| Open Trade with merchant.
		/if (${Target.Distance} < 20) {
		
			| Set timer
			/varset miscTimer 100
		
			| Right click merchant, and wait for window to open.
			:openMerchant
			/if (${Debug} || ${Debug_Sell}) /echo |- openMerchant -| :openMerchant
			
			/if (!${Window[MerchantWnd].Open}) /click right target
			/delay 5 ${Window[MerchantWnd].Open}
			/if (!${Window[MerchantWnd].Open}) {
				/if (${miscTimer}) {
					/goto :openMerchant
				} else {
					/echo Failed to open trade with [${Target.CleanName}].
				}
			}
			
			| Wait for merchant's item list to populate.
			:populateMerchant
			/if (${Debug} || ${Debug_Sell}) /echo |- openMerchant -| :populateMerchant
			/if (${merchantTotal}!=${Window[MerchantWnd].Child[ItemList].Items}) {
				/varset merchantTotal ${Window[MerchantWnd].Child[ItemList].Items}
				/delay 2
				/if (${miscTimer}) /goto :populateMerchant
			}
		}
	}

/if (${Debug} || ${Debug_Sell}) /echo <== openMerchant -|
/RETURN

|--------------------|
|- closeMerchant	-|
|--------------------|
SUB closeMerchant
/if (${Debug} || ${Debug_Sell}) /echo |- closeMerchant ==>

	/declare miscTimer timer local 50
	
	:closeMerchant
	/if (${Debug} || ${Debug_Sell}) /echo |- closeMerchant -| :closeMerchant
	/notify MerchantWnd MW_Done_Button leftmouseup
	/delay 5 ${Window[MerchantWnd].Open}
	/if (${Window[MerchantWnd].Open} && ${miscTimer}) /goto :closeMerchant
	
	
/if (${Debug} || ${Debug_Sell}) /echo <== closeMerchant -|
/RETURN

|-----------------------------------------------------------------------------------------------------------------------------------------|
|- Create an array of your inventory
|-----------------------------------------------------------------------------------------------------------------------------------------|
| Scan Inventory creates arrays of your inventory so that the code is a bit more readable.  Iterating inventory typically takes two passes
| one pass for containers and one pass for inventory slots.  The array just stores the location types so we do a single loop and try to 
| make the code a bit more readable.  Scan inventory will only keep track of items and ignores empty slots.
sub ScanInventory
/if (${Debug} || ${Debug_Sell}) /echo |- ScanInventory ==>
	/declare i int local
	/declare j int local
	/declare invSlot int local
	/declare containerSize int local
	/declare arraySizeNeeded int local 0
	/declare arrayCounter int local 1
	
	/if (${Defined[packArray]}) { /deletevar packArray }
	/if (${Defined[slotArray]}) { /deletevar slotArray }
	
	/echo Scanning Inventory
	/echo Calculating array size needed
	/for i 1 to ${numInventorySlots} {
		/varcalc invSlot ${Math.Calc[${i}+22]}
		
		/if (${Bool[${Me.Inventory[${invSlot}].Container}]}) {
			/varset containerSize ${Me.Inventory[${invSlot}].Container}
			/for j 1 to ${containerSize}
				/if (${Bool[${Me.Inventory[${invSlot}].Item[${j}]}]}) {
					/varcalc arraySizeNeeded ${Math.Calc[${arraySizeNeeded}+1]}
				}
			/next j
		} else {
			/if (${Bool[${Me.Inventory[${invSlot}]}]}) {
				/varcalc arraySizeNeeded ${Math.Calc[${arraySizeNeeded}+1]}
			}
		}
	/next i
	
	/declare packArray[${arraySizeNeeded}] string outer
	/declare slotArray[${arraySizeNeeded}] string outer

	/echo Inserting items into Arrays
	/for i 1 to ${numInventorySlots} {
		/varcalc invSlot ${Math.Calc[${i}+22]}
		
		/if (${Bool[${Me.Inventory[${invSlot}].Container}]}) {
			/varset containerSize ${Me.Inventory[${invSlot}].Container}
			/for j 1 to ${containerSize}
				/if (${Bool[${Me.Inventory[${invSlot}].Item[${j}]}]}) {
					/varset packArray[${arrayCounter}] ${i}
					/varset slotArray[${arrayCounter}] ${j}
					/varcalc arrayCounter ${Math.Calc[${arrayCounter}+1]}
				}
			/next j
		} else {
			/if (${Bool[${Me.Inventory[${invSlot}]}]}) {
				/varset packArray[${arrayCounter}] ${i}
				/varset slotArray[${arrayCounter}] 0
				/varcalc arrayCounter ${Math.Calc[${arrayCounter}+1]}
			}
		}
	/next i
	/echo Scan Inventory complete
/if (${Debug} || ${Debug_Sell}) /echo <== sellItems -|
/Return

Sub sellItems
/if (${Debug} || ${Debug_Sell}) /echo |- ScanInventory ==>
	/declare i int local
	| maxClickAttempts is our retry counter.  Latency causes things not to work so we will retry a certain number of times to do our best to make sure items get sold or destroyed
	/declare maxClickAttempts int local 10
	/declare clickAttempts int local 0
	
	/echo Selling Items ${GameTime.Time12} --- ${GameTime.SecondsSinceMidnight}
	
	| Scan your inventory into your pack and slot arrays
	/call ScanInventory
	/echo ScanInventory Complete ${GameTime.Time12} --- ${GameTime.SecondsSinceMidnight}
	
	| If your inventory is empty just jump out
	/if (${packArray.Size} == 0) /goto :sellEmptyInventory
	
	| Loop through all items that we scanned into the array
	/for i 1 to ${packArray.Size} {
		/call get_lootSetting pack${packArray[${i}]} ${slotArray[${i}]}
		/if (${lootSetting.Find[Sell]}) {	

			/varset clickAttempts 0
			| With the merchant window open, we'll left click the items to add them to the merchant sell window
			:selectItemAgain
			/if (${slotArray[${i}].Equal[0]}) {
				/itemnotify ${Math.Calc[${packArray[${i}]}+22]} leftmouseup
			} else {
				/itemnotify in pack${packArray[${i}]} ${slotArray[${i}]} leftmouseup
			}
			/delay 1
	
			| Check to see if the sell button is enabled once the merchant window is open
			/if (!${Window[MerchantWnd].Child[MW_Sell_Button].Enabled} && (${clickAttempts} < ${maxClickAttempts})) {
				/varset clickAttempts ${Math.Calc[${clickAttempts}+1]}
				/goto :selectItemAgain
			}
			
			| If we hit max click attempts print this message for now.  Probably move this to a debug only message eventually.
			/if (${clickAttempts} >= ${maxClickAttempts}) /echo Max clickAttempts reached on :selectItemAgain ${itemNameArray[${i}]}
			
			| The item should be in the merchant window at this point, and the sell button is enabled, so let's click the sell button
			| we're trying to sell with Shift key held down so that we don't get the quantity window
			/varset clickAttempts 0
			:pressSellAgain
			/shiftkey /notify MerchantWnd MW_Sell_Button leftmouseup
			/delay 1
			/if (${Window[MerchantWnd].Child[MW_Sell_Button].Enabled} && !${Window[QuantityWnd].Open} && (${clickAttempts} < ${maxClickAttempts})) {
				/varset clickAttempts ${Math.Calc[${clickAttempts}+1]}
				/goto :pressSellAgain
			}
			/if (${clickAttempts} >= ${maxClickAttempts}) /echo Max clickAttempts reached on :pressSellAgain ${i}
			/delay 1
			/varset clickAttempts 0
			:clickQuantityAgain
			| We used /shiftkey so hopefully the quantity window doesn't come up, but let's check just in case.
			/if (${Window[QuantityWnd].Open} && (${clickAttempts} < ${maxClickAttempts})) {
				/echo Quantity window popped up.
				/notify QuantityWnd QTYW_Accept_Button leftmouseup
				/delay 1
				/varset clickAttempts ${Math.Calc[${clickAttempts}+1]}
				/goto :clickQuantityAgain
			}
			/if (${clickAttempts} >= ${maxClickAttempts}) /echo Max clickAttempts reached on :clickQuantityAgain ${i}
			/delay 3
		}
	/next i
	:sellEmptyInventory
	/echo Sell Inventory Complete ${GameTime.Time12} --- ${GameTime.SecondsSinceMidnight}
/if (${Debug} || ${Debug_Sell}) /echo <== sellItems -|
/return

Sub destroyItems
/if (${Debug} || ${Debug_Sell}) /echo |- destroyItems ==>
	/declare i int local
	/declare maxClickAttempts int local 10
	/declare clickAttempts int local 0
	
	/echo Destroying Items
	/call ScanInventory
	/if (${packArray.Size} == 0) /goto destroyEmptyInventory
	/for i 1 to ${packArray.Size} {
		/call get_lootSetting pack${packArray[${i}]} ${slotArray[${i}]}
		/if (${lootSetting.Find[Destroy]}) {
			
			/varset clickAttempts 0
			:selectItemAgain
			/if (${slotArray[${i}].Equal[0]}) {
				/shiftkey /itemnotify ${Math.Calc[${packArray[${i}]}+22]} leftmouseup
			} else {
				/shiftkey /itemnotify in pack${packArray[${i}]} ${slotArray[${i}]} leftmouseup
			}
			/delay 1	
			
			/varset clickAttempts 0
			:clickQuantityAgain
			/if (${Window[QuantityWnd].Open} && (${clickAttempts} < ${maxClickAttempts})) {
				/notify QuantityWnd QTYW_Accept_Button leftmouseup
				/delay 1
				/varset clickAttempts ${Math.Calc[${clickAttempts}+1]}
				/goto :clickQuantityAgain
			}
			/if (${clickAttempts} >= ${maxClickAttempts}) /echo Max clickAttempts reached on :clickQuantityAgain
			
			/if (!${Cursor.ID} && (${clickAttempts} < ${maxClickAttempts})) {
				/varset clickAttempts ${Math.Calc[${clickAttempts}+1]}
				/goto :selectItemAgain
			}
			/if (${clickAttempts} >= ${maxClickAttempts}) /echo Max clickAttempts reached on :selectItemAgain
			/delay 1			
			/destroy
			/delay 3
			/autoinventory
		}
	/next i
	:destroyEmptyInventory
/if (${Debug} || ${Debug_Sell}) /echo <== destroyItems -|
/return

SUB sell_Setup
	/call iniToVarV "${advSettings_Ini},Debug,Debug Sell (On/Off)" Debug_Sell bool outer
	/if (${Debug} || ${Debug_Sell}) /echo |- sell_Setup ==>
	
	|- These outer variables are defined because offer and sold events happen independently of each other.  
	|- The offer message stores the autoFlagSoldItemEntry and when the sold event triggers, the last offer for an item by the vendor is used to identify the item.
	
	/call WriteToIni "${advSettings_Ini},Debug,Debug Sell (On/Off)" "Off" False
	/call WriteToIni "${genSettings_Ini},Sell,Destroy Unsold Items(On/Off)" "Off" False
	
	/call iniToVarV "${genSettings_Ini},Sell,Destroy Unsold Items(On/Off)" destroyUnsold bool outer
	
	/call sell_Aliases
	/if (${Debug} || ${Debug_Sell}) /echo <== sell_Setup -|
/RETURN

Sub sell_Background_Events
	/doevents syncInventory
	/doevents sellItems
	/doevents Combine
	/doevents printItemStatus
	/doevents keepthisitem
	/doevents sellthisitem
	/doevents destroythisitem
/return

Sub sell_Aliases
/if (${Debug || ${Debug_Sell}}) /echo |- _Aliases ==>
	/squelch /alias /syncInventory /bc Sync Inventory
	/squelch /alias /autosell /echo Auto Sell
	/squelch /alias /printitemstatus /bc printitemstatus
	/squelch /alias /sellthisitem /bc sellthisitem
	/squelch /alias /keepthisitem /bc keepthisitem
	/squelch /alias /destroythisitem /bc destroythisitem
/if (${Debug || ${Debug_Sell}}) /echo <== _Aliases -|
/return

SUB sell_CharacterSettings
/RETURN

SUB sell_MacroSettings
    /squelch /alias /syncInventory /bc Sync Inventory
/if (${Debug}) /echo |- _MacroSettings ==>
    /call WriteToIni "${advSettings_Ini},Debug,Debug Sell (On/Off)" Off
    /call WriteToIni "${genSettings_Ini},Sell,Destroy Unsold Items(On/Off)" Off
    /squelch /alias /sellthisitem /bc sellthisitem
    /squelch /alias /keepthisitem /bc keepthisitem
    /squelch /alias /printitemstatus /bc printitemstatus
    /squelch /alias /destroythisitem /bc destroythisitem
    /squelch /alias /autosell /echo Auto Sell
/if (${Debug}) /echo <== _MacroSettings -|
/RETURN

|- Gets the Loot Setting value for the item name passed in.
SUB getIniKey(originalName)
/if (${Debug || ${Debug_Sell}}) /echo |- getIniKey ${originalName}==>
	/declare itemName string local
	/declare itemValue string local
  	/declare itemRawValue int local
	/declare iniEntryVariables string local

	/varset itemName ${originalName}

	/if (${itemName.Find[:]}) /varset itemName ${itemName.Replace[:,;]}
    /if (${itemName.Find[,]}) {
      	/call RemoveComma "${itemName}"
      	/varset itemName ${Macro.Return}
    }
	
	| Set item value
	/varset itemValue ${FindItem[${originalName}].Value}
    /varset itemRawValue ${FindItem[${originalName}].Value}
    /varset itemValue ${If[${Bool[${itemValue.Left[${Math.Calc[${itemValue.Length} - 3].Int}]}]},${itemValue.Left[${Math.Calc[${itemValue.Length} - 3].Int}]}p,]}${If[${Bool[${itemValue.Mid[${Math.Calc[${itemValue.Length} - 2].Int}]}]},${itemValue.Mid[${Math.Calc[${itemValue.Length} - 2].Int}]}g,]}${If[${Bool[${itemValue.Mid[${Math.Calc[${itemValue.Length} - 1].Int}]}]},${itemValue.Mid[${Math.Calc[${itemValue.Length} - 1].Int}]}s,]}${If[${Bool[${itemValue.Right[1]}]},${itemValue.Right[1]}c,]}
		
	| Set ini variables like stack size, (C), (ND) etc.
	/varset iniEntryVariables ${If[${FindItem[${originalName}].Stackable},(${FindItem[${originalName}].StackSize}),]}${If[${FindItem[${originalName}].NoDrop},(ND),]}${If[${FindItem[${originalName}].Lore},(L),]}${If[${FindItem[${originalName}].Container},(C),]}

	/return ${itemName.Left[1]},${itemName} ${itemValue}${iniEntryVariables}
/if (${Debug || ${Debug_Sell}}) /echo <== getIniKey -|
/RETURN
