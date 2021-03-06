include_directories(${CMAKE_HOME_DIRECTORY} ${CMAKE_HOME_DIRECTORY}/include ${CMAKE_CURRENT_SOURCE_DIRECTORY} ${CMAKE_HOME_DIRECTORY}/lib)
include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR})

set(lib_SRCS
		StdInc.cpp
		${CMAKE_BINARY_DIR}/Version.cpp

		battle/AccessibilityInfo.cpp
		battle/BattleAction.cpp
		battle/BattleAttackInfo.cpp
		battle/BattleHex.cpp
		battle/BattleInfo.cpp
		battle/BattleProxy.cpp
		battle/CBattleInfoCallback.cpp
		battle/CBattleInfoEssentials.cpp
		battle/CCallbackBase.cpp
		battle/CObstacleInstance.cpp
		battle/CPlayerBattleCallback.cpp
		battle/CUnitState.cpp
		battle/Destination.cpp
		battle/IBattleState.cpp
		battle/ReachabilityInfo.cpp
		battle/SideInBattle.cpp
		battle/SiegeInfo.cpp
		battle/Unit.cpp

		filesystem/AdapterLoaders.cpp
		filesystem/CArchiveLoader.cpp
		filesystem/CBinaryReader.cpp
		filesystem/CCompressedStream.cpp
		filesystem/CFileInputStream.cpp
		filesystem/CFilesystemLoader.cpp
		filesystem/CMemoryBuffer.cpp
		filesystem/CMemoryStream.cpp
		filesystem/CZipLoader.cpp
		filesystem/CZipSaver.cpp
		filesystem/FileInfo.cpp
		filesystem/FileStream.cpp
		filesystem/Filesystem.cpp
		filesystem/MinizipExtensions.cpp
		filesystem/ResourceID.cpp

		logging/CBasicLogConfigurator.cpp
		logging/CLogger.cpp

		mapObjects/CArmedInstance.cpp
		mapObjects/CBank.cpp
		mapObjects/CGHeroInstance.cpp
		mapObjects/CGMarket.cpp
		mapObjects/CGPandoraBox.cpp
		mapObjects/CGTownInstance.cpp
		mapObjects/CObjectClassesHandler.cpp
		mapObjects/CObjectHandler.cpp
		mapObjects/CommonConstructors.cpp
		mapObjects/CQuest.cpp
		mapObjects/CRewardableConstructor.cpp
		mapObjects/CRewardableObject.cpp
		mapObjects/JsonRandom.cpp
		mapObjects/MiscObjects.cpp
		mapObjects/ObjectTemplate.cpp

		mapping/CCampaignHandler.cpp
		mapping/CDrawRoadsOperation.cpp
		mapping/CMap.cpp
		mapping/CMapEditManager.cpp
		mapping/CMapInfo.cpp
		mapping/CMapService.cpp
		mapping/MapFormatH3M.cpp
		mapping/MapFormatJson.cpp

		registerTypes/RegisterTypes.cpp
		registerTypes/TypesClientPacks1.cpp
		registerTypes/TypesClientPacks2.cpp
		registerTypes/TypesMapObjects1.cpp
		registerTypes/TypesMapObjects2.cpp
		registerTypes/TypesMapObjects3.cpp
		registerTypes/TypesPregamePacks.cpp
		registerTypes/TypesServerPacks.cpp

		rmg/CMapGenerator.cpp
		rmg/CMapGenOptions.cpp
		rmg/CRmgTemplate.cpp
		rmg/CRmgTemplateStorage.cpp
		rmg/CRmgTemplateZone.cpp
		rmg/CZoneGraphGenerator.cpp
		rmg/CZonePlacer.cpp

		serializer/BinaryDeserializer.cpp
		serializer/BinarySerializer.cpp
		serializer/CLoadIntegrityValidator.cpp
		serializer/CMemorySerializer.cpp
		serializer/Connection.cpp
		serializer/CSerializer.cpp
		serializer/CTypeList.cpp
		serializer/JsonDeserializer.cpp
		serializer/JsonSerializeFormat.cpp
		serializer/JsonSerializer.cpp

		spells/AdventureSpellMechanics.cpp
		spells/BattleSpellMechanics.cpp
		spells/CSpellHandler.cpp
		spells/ISpellMechanics.cpp
		spells/Problem.cpp
		spells/TargetCondition.cpp
		spells/ViewSpellInt.cpp

		spells/effects/Catapult.cpp
		spells/effects/Clone.cpp
		spells/effects/Damage.cpp
		spells/effects/Dispel.cpp
		spells/effects/Effect.cpp
		spells/effects/Effects.cpp
		spells/effects/Heal.cpp
		spells/effects/LocationEffect.cpp
		spells/effects/Obstacle.cpp
		spells/effects/Registry.cpp
		spells/effects/UnitEffect.cpp
		spells/effects/Summon.cpp
		spells/effects/Teleport.cpp
		spells/effects/Timed.cpp
		spells/effects/RemoveObstacle.cpp
		spells/effects/Sacrifice.cpp

		CAndroidVMHelper.cpp
		CArtHandler.cpp
		CBonusTypeHandler.cpp
		CBuildingHandler.cpp
		CConfigHandler.cpp
		CConsoleHandler.cpp
		CCreatureHandler.cpp
		CCreatureSet.cpp
		CGameInfoCallback.cpp
		CGameInterface.cpp
		CGameState.cpp
		CGeneralTextHandler.cpp
		CHeroHandler.cpp
		CModHandler.cpp
		CPathfinder.cpp
		CRandomGenerator.cpp
		CSkillHandler.cpp
		CStack.cpp
		CThreadHelper.cpp
		CTownHandler.cpp
		GameConstants.cpp
		HeroBonus.cpp
		IGameCallback.cpp
		IHandlerBase.cpp
		JsonDetail.cpp
		JsonNode.cpp
		LogicalExpression.cpp
		NetPacksLib.cpp
		ResourceSet.cpp
		VCMIDirs.cpp
		VCMI_Lib.cpp

		${VCMILIB_ADDITIONAL_SOURCES}
)

set(lib_HEADERS
		StdInc.h
		../Global.h

		battle/AccessibilityInfo.h
		battle/BattleAction.h
		battle/BattleAttackInfo.h
		battle/BattleHex.h
		battle/BattleInfo.h
		battle/BattleProxy.h
		battle/CBattleInfoCallback.h
		battle/CBattleInfoEssentials.h
		battle/CCallbackBase.h
		battle/CObstacleInstance.h
		battle/CPlayerBattleCallback.h
		battle/CUnitState.h
		battle/Destination.h
		battle/IBattleState.h
		battle/IUnitInfo.h
		battle/ReachabilityInfo.h
		battle/SideInBattle.h
		battle/SiegeInfo.h
		battle/Unit.h

		filesystem/AdapterLoaders.h
		filesystem/CArchiveLoader.h
		filesystem/CBinaryReader.h
		filesystem/CCompressedStream.h
		filesystem/CFileInputStream.h
		filesystem/CFilesystemLoader.h
		filesystem/CInputOutputStream.h
		filesystem/CInputStream.h
		filesystem/CMemoryBuffer.h
		filesystem/CMemoryStream.h
		filesystem/COutputStream.h
		filesystem/CStream.h
		filesystem/CZipLoader.h
		filesystem/CZipSaver.h
		filesystem/FileInfo.h
		filesystem/FileStream.h
		filesystem/Filesystem.h
		filesystem/ISimpleResourceLoader.h
		filesystem/MinizipExtensions.h
		filesystem/ResourceID.h

		logging/CBasicLogConfigurator.h
		logging/CLogger.h

		mapObjects/CArmedInstance.h
		mapObjects/CBank.h
		mapObjects/CGHeroInstance.h
		mapObjects/CGMarket.h
		mapObjects/CGPandoraBox.h
		mapObjects/CGTownInstance.h
		mapObjects/CObjectClassesHandler.h
		mapObjects/CObjectHandler.h
		mapObjects/CommonConstructors.h
		mapObjects/CQuest.h
		mapObjects/CRewardableConstructor.h
		mapObjects/CRewardableObject.h
		mapObjects/JsonRandom.h
		mapObjects/MapObjects.h
		mapObjects/MiscObjects.h
		mapObjects/ObjectTemplate.h

		mapping/CCampaignHandler.h
		mapping/CDrawRoadsOperation.h
		mapping/CMapDefines.h
		mapping/CMapEditManager.h
		mapping/CMap.h
		mapping/CMapInfo.h
		mapping/CMapService.h
		mapping/MapFormatH3M.h
		mapping/MapFormatJson.h

		registerTypes/RegisterTypes.h

		rmg/CMapGenerator.h
		rmg/CMapGenOptions.h
		rmg/CRmgTemplate.h
		rmg/CRmgTemplateStorage.h
		rmg/CRmgTemplateZone.h
		rmg/CZoneGraphGenerator.h
		rmg/CZonePlacer.h
		rmg/float3.h

		serializer/BinaryDeserializer.h
		serializer/BinarySerializer.h
		serializer/CLoadIntegrityValidator.h
		serializer/CMemorySerializer.h
		serializer/Connection.h
		serializer/CSerializer.h
		serializer/CTypeList.h
		serializer/JsonDeserializer.h
		serializer/JsonSerializeFormat.h
		serializer/JsonSerializer.h

		spells/AdventureSpellMechanics.h
		spells/BattleSpellMechanics.h
		spells/CSpellHandler.h
		spells/ISpellMechanics.h
		spells/Magic.h
		spells/SpellMechanics.h
		spells/Problem.h
		spells/TargetCondition.h
		spells/ViewSpellInt.h

		spells/effects/Catapult.h
		spells/effects/Clone.h
		spells/effects/Damage.h
		spells/effects/Dispel.h
		spells/effects/Effect.h
		spells/effects/Effects.h
		spells/effects/EffectsFwd.h
		spells/effects/Heal.h
		spells/effects/LocationEffect.h
		spells/effects/Obstacle.h
		spells/effects/Registry.h
		spells/effects/UnitEffect.h
		spells/effects/Summon.h
		spells/effects/Teleport.h
		spells/effects/Timed.h
		spells/effects/RemoveObstacle.h
		spells/effects/Sacrifice.h
		AI_Base.h
		CAndroidVMHelper.h
		CArtHandler.h
		CBonusTypeHandler.h
		CBuildingHandler.h
		CConfigHandler.h
		CConsoleHandler.h
		CCreatureHandler.h
		CCreatureSet.h
		CGameInfoCallback.h
		CGameInterface.h
		CGameStateFwd.h
		CGameState.h
		CGeneralTextHandler.h
		CHeroHandler.h
		CModHandler.h
		CondSh.h
		ConstTransitivePtr.h
		CPathfinder.h
		CPlayerState.h
		CRandomGenerator.h
		CScriptingModule.h
		CSkillHandler.h
		CSoundBase.h
		CStack.h
		CStopWatch.h
		CThreadHelper.h
		CTownHandler.h
		FunctionList.h
		GameConstants.h
		HeroBonus.h
		IBonusTypeHandler.h
		IGameCallback.h
		IGameEventsReceiver.h
		IHandlerBase.h
		int3.h
		Interprocess.h
		JsonDetail.h
		JsonNode.h
		LogicalExpression.h
		NetPacksBase.h
		NetPacks.h
		ResourceSet.h
		ScopeGuard.h
		StartInfo.h
		StringConstants.h
		UnlockGuard.h
		VCMIDirs.h
		vcmi_endian.h
		VCMI_Lib.h
)

assign_source_group(${lib_SRCS} ${lib_HEADERS})

add_library(vcmi SHARED ${lib_SRCS} ${lib_HEADERS})
set_target_properties(vcmi PROPERTIES COMPILE_DEFINITIONS "VCMI_DLL=1")
target_link_libraries(vcmi ${MINIZIP_LIBRARIES} ${Boost_LIBRARIES} ${SDL2_LIBRARY} ${ZLIB_LIBRARIES} ${SYSTEM_LIBS})

if(WIN32)
	set_target_properties(vcmi
		PROPERTIES
			OUTPUT_NAME "VCMI_lib"
			PROJECT_LABEL "VCMI_lib"
	)
endif()

if(ANDROID)
	return()
endif()

vcmi_set_output_dir(vcmi "")

set_target_properties(vcmi PROPERTIES ${PCH_PROPERTIES})
cotire(vcmi)

# We want to deploy assets into build directory for easier debugging without install
add_custom_command(TARGET vcmi POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/config
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/Mods
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/config ${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/config
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Mods ${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INTDIR}/Mods
)

install(TARGETS vcmi RUNTIME DESTINATION ${LIB_DIR} LIBRARY DESTINATION ${LIB_DIR})
