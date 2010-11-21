

z = locals().copy()

for x in z:
	print "%020s, %s" % (x, z[x])

	
q = mq2.Character()

print q.SpellGem(3)


print TLO.Me.Heading.DegreesCCW()
print TLO.Spell(254).Name()


def OpenSpellBook():
	if not TLO.Window["SpellBookWnd"].Open():
		DoCommand("/keypress spellbok")
		
OpenSpellBook()