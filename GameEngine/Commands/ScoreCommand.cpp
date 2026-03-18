#include "ScoreCommand.h"
#include "GameObject.h"
#include "Components/ScoreComponent.h"

using namespace bombGame;

ScoreCommand::ScoreCommand(ge::GameObject* commandTarget, int scoreDiff)
	:ge::GameObjectCommand::GameObjectCommand(commandTarget),
	m_ScoreDiff{ scoreDiff }
{
}

void ScoreCommand::Execute(float)
{
	auto* scoreComp{ GetCommandTarget()->GetComponent<ScoreComponent>() };

	if (scoreComp)
		scoreComp->ChangeScore(m_ScoreDiff);
}
